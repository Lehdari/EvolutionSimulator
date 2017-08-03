#ifndef WORLD_HPP
#define WORLD_HPP


#include "QuadTree.hpp"
#include "ObjectPool.hpp"
#include "Creature.hpp"
#include "Food.hpp"
#include "ResourceMap.hpp"
#include "Evolver.hpp"


class World {
public:
    struct Hit {
        enum Type {
            NO_HIT,
            FOOD
        } type;
        float t;
    };

    World(uint64_t width, uint64_t height);

    void tick(void);
    void render(sf::RenderTarget& target);

private:
    uint64_t    _width;
    uint64_t    _height;

    sf::Texture _creatureTex;
    sf::Texture _foodTex;

    QuadTree<std::vector, RenderableCreature>   _creatures;
    QuadTree<ObjectPool, Food>                  _food;

    ResourceMap _resourceMap;
    Evolver     _evolver;

    //  statistics
    struct Statistics {
        uint64_t    totalFoodCollected = 0;
        uint64_t    maximumFoodCollected = 0;
        uint64_t    deaths = 0;
    } _statistics;

    enum ObjectType {
        NONE        = 0,
        CREATURE    = 1,
        FOOD        = 2
    };

    struct CreatureWhiskerDetection {
        ObjectType  type = NONE;
        float       t = 1.0f;  //  whisker ray length (<1.0 means detection)
        void*       object = nullptr;   //  pointer to the detected object
    };

    struct CreatureCollision {
        Creature*   creature;
        Creature*   otherCreature;

        CreatureCollision(Creature* c, Creature* oc) :
            creature(c), otherCreature(oc) {}
    };

    struct FoodCollision {
        Creature*   creature;
        ObjectPool<Food>::Iterator  food;

        FoodCollision(Creature* c, ObjectPool<Food>::Iterator& f) :
            creature(c), food(f) {}
    };

    Vec2f rndCreaturePosition(void);
};


#endif // WORLD_HPP
