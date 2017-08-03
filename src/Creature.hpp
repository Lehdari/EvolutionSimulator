#ifndef CREATURE_HPP
#define CREATURE_HPP


#include <SFML/Graphics.hpp>

#include "MathTypes.hpp"
#include "RNN.hpp"
#include "QuadTree.hpp"


class Creature : public QuadTreeObject {
public:
    friend class World;
    friend class Evolver;

    Creature(const RNN& rnn, const RNN::Genome& genome,
        const Vec2f& pos, float r, float g, float b, double life);

    void respawn(const Vec2f& pos, double life);

    void tick(void);

    void applyColorMutation(void);

    Vec2f getPos(void) const;
    Vec2f getWhiskerHead(void) const;

protected:
    RNN         _rnn;
    RNN::Genome _genome;
    uint64_t    _nNeurons;

    float       _dir;
    float       _spd;

    float       _whiskerFactor;    //  between 0 and 1, multiplied by maxWhiskerLen
    Vec2f       _whiskerVec;

    float       _r;
    float       _g;
    float       _b;

    double      _life;  //  life left
    uint64_t    _lifeElapsed;
    uint64_t    _foodCollected;
};

class RenderableCreature : public Creature {
public:
    friend class World;
    friend class Evolver;

    RenderableCreature(const RNN& rnn, const RNN::Genome& genome,
        const Vec2f& pos, float r, float g, float b, int64_t life,
        const sf::Texture& tex);

    void tick(void);

    void render(sf::RenderTarget& target);

    void setWhiskerColor(const sf::Color& color);
    void setWhiskerLength(float t);

private:
    sf::Sprite      _spr;
    sf::VertexArray _whisker;
};


#endif // CREATURE_HPP
