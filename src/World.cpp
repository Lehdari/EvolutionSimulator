#include "World.hpp"
#include "RNNMaker.hpp"
#include "Random.hpp"
#include "Globals.hpp"


#include <cmath>


namespace {

    inline float rayCircleIntersection(
        const Vec2f& ro, const Vec2f& rd, const Vec2f& co, float cr)
    {
        float crdx = ro(0) - co(0);
        float crdy = ro(1) - co(1);
        float a = rd(0)*rd(0) + rd(1)*rd(1);
        float b = 2.0f*(rd(0)*crdx + rd(1)*crdy);
        float c = crdx*crdx + crdy*crdy - cr*cr;

        float dcn = b*b - 4*a*c;
        if (dcn < 0.0f)
            return 1.0f;

        float t = (-b-std::sqrt(dcn))/(2.0f*a);
        if (t <= 1.0f && t>= 0.0f)
            return t;

        return 1.0f;
    }

}


World::World(uint64_t width, uint64_t height) :
    _width          (width),
    _height         (height),
    _creatures      (globals::quadTreeDepth, globals::worldW, globals::worldH),
    _food           (globals::quadTreeDepth, globals::worldW, globals::worldH,
                     globals::maxFoodNumber),
    _resourceMap    (globals::foodFertilityMapFileName, _width, _height),
    _evolver        (_creatures._objects)
{
    _creatureTex.loadFromFile("res/creature.png");
    _foodTex.loadFromFile("res/food.png");

    for (auto i=0llu; i<globals::nCreatures; ++i) {
        RNNMaker rnnm(7, 4);
        rnnm.createCircularLayeredRNN(5, 12);

        _creatures._objects.emplace_back(rnnm.getRNN(), rnnm.getGenome(),
            rndCreaturePosition(),
            RNDF, RNDF, RNDF,
            RNDRANGE(globals::minCreatureInitLife, globals::maxCreatureInitLife),
            _creatureTex);
    }

    for (auto i=0llu; i<globals::initFoodNumber; ++i) {
        _food._objects.emplaceFirstDeactive(_foodTex, _resourceMap.getRandomFoodPos());
    }
}

void World::tick(void)
{
    //  recalculate quadtrees
    _creatures.recalculate();
    _food.recalculate();

    //  replace the dead
    for (auto cId = 0llu; cId < _creatures._objects.size(); ++cId) {
        auto& c = _creatures._objects[cId];

        if (c._life <= 0) {
            _statistics.totalFoodCollected += c._foodCollected;
            ++_statistics.deaths;

            if (c._foodCollected > _statistics.maximumFoodCollected) {
                _statistics.maximumFoodCollected = c._foodCollected;
            }

            //  TEMP
            /*printf("Food collected by creature %llu: %llu, maximum: %llu, average: %0.3f\n",
                cId, c._foodCollected,
                _statistics.maximumFoodCollected,
                (double)_statistics.totalFoodCollected/(double)_statistics.deaths
                );*/

            auto parentId = _evolver.replaceRNN(cId);

            //  respawn at position of the parent
            auto parentPos = _creatures._objects[parentId].getPos();
            auto parentLife = _creatures._objects[parentId]._life;
            c.respawn(_creatures._objects[parentId].getPos(),
                globals::minCreatureInitLife + parentLife*0.5);
        }
    }

    //  update creatures
    #pragma omp parallel for
    for (auto cIt = _creatures._objects.begin(); cIt < _creatures._objects.end(); ++cIt) {
        cIt->tick();
    }

    //  collisions and whisker detections
    static std::vector<CreatureWhiskerDetection> detections(_creatures._objects.size());
    static std::vector<CreatureCollision> creatureCollisions;
    static std::vector<FoodCollision> foodCollisions;


    #pragma omp parallel for
    for (auto dIt = detections.begin(); dIt < detections.end(); ++dIt)
        *dIt = CreatureWhiskerDetection();

    creatureCollisions.clear();
    foodCollisions.clear();

    #pragma omp parallel for
    for (auto cId = 0llu; cId < _creatures._objects.size(); ++cId) {
        auto& c = _creatures._objects[cId];
        auto cPos = c.getPos();
        //auto cWhiskerHead = c.getWhiskerHead();

        //  containers for collision check / detection
        std::vector<std::vector<RenderableCreature>::iterator>  creatures;
        std::vector<ObjectPool<Food>::Iterator>  food;

        //  creatures collisions
        _creatures.getObjects(creatures, cPos(0)-32, cPos(0)+32, cPos(1)-32, cPos(1)+32);
        for (auto cIt : creatures) {
            //  don't check creature itself
            if (&*cIt == &c)
                continue;

            auto cPos2 = cIt->getPos();

            //  collision with creatures
            if ((cPos - cPos2).squaredNorm() < 1024.0f) {
                #pragma omp critical
                creatureCollisions.emplace_back(&c, &*cIt);
            }
        }

        //  creatures detection
        double xfMin = std::min(cPos(0), cPos(0)+c._whiskerVec(0))-16.0f;
        double xfMax = std::max(cPos(0), cPos(0)+c._whiskerVec(0))+16.0f;
        double yfMin = std::min(cPos(1), cPos(1)+c._whiskerVec(1))-16.0f;
        double yfMax = std::max(cPos(1), cPos(1)+c._whiskerVec(1))+16.0f;
        creatures.clear();
        _creatures.getObjects(creatures, xfMin, xfMax, yfMin, yfMax);
        for (auto cIt : creatures) {
            //  don't check creature itself
            if (&*cIt == &c)
                continue;

            auto cPos2 = cIt->getPos();

            //  whisker ray cast
            float t = rayCircleIntersection(cPos,
                c._whiskerVec*detections[cId].t, cPos2, 16.0f);
            if (t < 1.0f) {
                detections[cId].type = CREATURE;
                detections[cId].t *= t;
                detections[cId].object = &*cIt;
            }
        }

        //  food collisions
        _food.getObjects(food, cPos(0)-24, cPos(0)+24, cPos(1)-24, cPos(1)+24);
        for (auto fIt : food) {
            auto fPos = fIt->getPos();

            //  collision with food
            if ((cPos - fPos).squaredNorm() < 784) {  //  (12+16)^2
                #pragma omp critical
                foodCollisions.emplace_back(&c, fIt);
            }
        }

        //  food detection
        xfMin = std::min(cPos(0), cPos(0)+(c._whiskerVec*detections[cId].t)(0))-12.0f;
        xfMax = std::max(cPos(0), cPos(0)+(c._whiskerVec*detections[cId].t)(0))+12.0f;
        yfMin = std::min(cPos(1), cPos(1)+(c._whiskerVec*detections[cId].t)(1))-12.0f;
        yfMax = std::max(cPos(1), cPos(1)+(c._whiskerVec*detections[cId].t)(1))+12.0f;
        food.clear();
        _food.getObjects(food, xfMin, xfMax, yfMin, yfMax);
        for (auto fIt : food) {
            auto fPos = fIt->getPos();

            //  whisker ray cast
            float t = rayCircleIntersection(cPos,
                c._whiskerVec*detections[cId].t, fPos, 12.0f);
            if (t < 1.0f) {
                detections[cId].type = FOOD;
                detections[cId].t *= t;
                detections[cId].object = &*fIt;
            }
        }
    }


    //  react to collisions
    for (auto& c : creatureCollisions) {
        auto cPos2 = c.otherCreature->getPos();
        auto dn = (c.creature->getPos() - cPos2).normalized();
        auto np = cPos2 + dn*32.0f;
        c.creature->_x = np(0);
        c.creature->_y = np(1);
    }

    for (auto& c : foodCollisions) {
        if (!c.food.isActive())
            continue;
        c.creature->_life += RNDRANGE(globals::minFoodLifeIncrease, globals::maxFoodLifeIncrease);
        ++c.creature->_foodCollected;
        c.food.deactivate();
    }


    //  update inputs and inflict damage
    #pragma omp parallel for
    for (auto cId = 0llu; cId < _creatures._objects.size(); ++cId) {
        auto& c = _creatures._objects[cId];
        auto& d = detections[cId];

        auto damage = c._rnn.getOutput(3)*5;
        c._life -= damage*0.1;

        /*
            input 0: fertility
            input 1: whisker ray length
            input 2: 1st bit of detected object type
            input 3: 2nd bit of detected object type
            input 4: red channel of detected creature color (0 if no creature detected)
            input 5: green channel of detected creature color (0 if no creature detected)
            input 6: blue channel of detected creature color (0 if no creature detected)
        */

        c._rnn.setInput(0, _resourceMap.getFoodFertility(c.getPos()));
        c._rnn.setInput(1, d.t);
        c._rnn.setInput(2, d.type & 1);
        c._rnn.setInput(3, (d.type>>1) & 1);

        if (d.type == CREATURE) {
            auto* c2 = static_cast<Creature*>(d.object);
            c._rnn.setInput(4, c2->_r);
            c._rnn.setInput(5, c2->_g);
            c._rnn.setInput(6, c2->_b);
            c.setWhiskerColor(sf::Color(c2->_r*255, c2->_g*255, c2->_b*255));
            c.setWhiskerLength(d.t);
            c2->_life -= damage;
        }
        else {
            c._rnn.setInput(4, 0.0f);
            c._rnn.setInput(5, 0.0f);
            c._rnn.setInput(6, 0.0f);
            c.setWhiskerColor(sf::Color(255, 255, 255));
            c.setWhiskerLength(d.t);
        }

    }

    //  TEMP
    static uint64_t fp = 1000;
    if (--fp <= 0) {
        fp = 1000;
        //printf("Active food: %llu\n", _food._objects.getNActive());
        _food._objects.tseg();
    }


    //  spawn food
    for (auto i=0llu; i<globals::foodPerTick &&
        _food._objects.getNActive() < globals::maxFoodNumber; ++i)
        _food._objects.emplaceFirstDeactive(
            _foodTex, _resourceMap.getRandomFoodPos());
}

void World::render(sf::RenderTarget& target)
{
    _resourceMap.render(target);

    for (auto& c : _creatures._objects)
        c.render(target);

    for (auto& f : _food._objects)
        f.render(target);
}

Vec2f World::rndCreaturePosition(void)
{
    return Vec2f(rnd()%(_width-200)+100, rnd()%(_height-200)+100);
}
