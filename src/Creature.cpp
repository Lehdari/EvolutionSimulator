#include "Creature.hpp"
#include "Random.hpp"
#include "Globals.hpp"


Creature::Creature(const RNN& rnn, const RNN::Genome& genome,
    const Vec2f& pos, float r, float g, float b, double life) :
    QuadTreeObject(pos(0), pos(1)),
    _rnn(rnn),
    _genome(genome),
    _nNeurons(_rnn.getNeurons().size()),
    _dir(RNDF*2.0*PI),
    _spd(0.0f),
    _whiskerFactor(0.0f),
    _whiskerVec(0.0f, 0.0f),
    _r(r), _g(g), _b(b),
    _life(life),
    _lifeElapsed(0),
    _foodCollected(0)
{
    //RNNMaker rnnm(1, 2);
    //rnnm.createCircularLayeredRNN(5, 12);
    //_genome = rnnm.getGenome();
    //_rnn = std::make_unique<RNN>(rnnm.getRNN());
}

void Creature::respawn(const Vec2f& pos, double life)
{
    _x = pos(0);
    _y = pos(1);
    _dir = RNDF*2.0*PI;
    _life = life;
    _foodCollected = 0;
}

void Creature::tick(void)
{
    _rnn.tick();
    _dir += (_rnn.getOutput(0)-0.5)*0.25;
    _spd = _rnn.getOutput(1)-0.25;
    _whiskerFactor = _rnn.getOutput(2);

    float cosd = cos(_dir);
    float sind = sin(_dir);

    _x += _spd*cosd*5.0f;
    _y += _spd*sind*5.0f;

    if (_x < 0) _x = 0;
    if (_x > globals::worldW) _x = globals::worldW;
    if (_y < 0) _y = 0;
    if (_y > globals::worldH) _y = globals::worldH;

    _whiskerVec(0) = _whiskerFactor*globals::maxWhiskerLen*cosd;
    _whiskerVec(1) = _whiskerFactor*globals::maxWhiskerLen*sind;

    //  penalize for moving and whisker length
    _life -= _whiskerFactor*_whiskerFactor*0.3 + (_spd-0.1)*(_spd-0.1)*2.5 + 0.5;
    ++_lifeElapsed;
}

void Creature::applyColorMutation(void)
{
    _r += (RNDF-0.5f)*0.02f;
    _g += (RNDF-0.5f)*0.02f;
    _b += (RNDF-0.5f)*0.02f;

    if (_r < 0.0f) _r = 0.0f;
    if (_g < 0.0f) _g = 0.0f;
    if (_b < 0.0f) _b = 0.0f;
    if (_r > 1.0f) _r = 1.0f;
    if (_g > 1.0f) _g = 1.0f;
    if (_b > 1.0f) _b = 1.0f;
}

Vec2f Creature::getPos(void) const
{
    return Vec2f(_x, _y);
}

Vec2f Creature::getWhiskerHead(void) const
{
    return getPos() + _whiskerVec;
}


RenderableCreature::RenderableCreature(const RNN& rnn, const RNN::Genome& genome,
    const Vec2f& pos, float r, float g, float b, int64_t life, const sf::Texture& tex) :
    Creature(std::move(rnn), std::move(genome), pos, r, g, b, life),
    _whisker(sf::Lines, 2)
{
    _spr.setTexture(tex);
    _spr.setOrigin(tex.getSize().x*0.5, tex.getSize().y*0.5);
}

void RenderableCreature::tick(void)
{
    Creature::tick();
    _spr.setPosition(_x, _y);
    _spr.setRotation(_dir*57.2957795f);
    _spr.setColor(sf::Color(_r*255, _g*255, _b*255));

    _whisker[0].position = sf::Vector2f(_x, _y);
    _whisker[1].position = sf::Vector2f(
        _x+_whiskerVec(0),
        _y+_whiskerVec(1));
}

void RenderableCreature::render(sf::RenderTarget& target)
{
    target.draw(_spr);
    target.draw(_whisker);
}

void RenderableCreature::setWhiskerColor(const sf::Color& color)
{
    _whisker[0].color = color;
    _whisker[1].color = color;
}

void RenderableCreature::setWhiskerLength(float t)
{
    _whisker[1].position = sf::Vector2f(_x+t*_whiskerVec(0), _y+t*_whiskerVec(1));
}
