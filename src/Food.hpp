#ifndef FOOD_HPP
#define FOOD_HPP


#include <SFML/Graphics.hpp>

#include "MathTypes.hpp"
#include "QuadTree.hpp"


class Food : public QuadTreeObject {
public:
    friend class World;

    Food(const sf::Texture& tex, const Vec2f& pos);

    void setPosition(const Vec2f& pos);

    void render(sf::RenderTarget& target);

    Vec2f getPos(void) const;

private:
    sf::Sprite  _spr;
};


#endif // FOOD_HPP
