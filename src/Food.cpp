#include "Food.hpp"


Food::Food(const sf::Texture& tex, const Vec2f& pos) :
    QuadTreeObject(pos(0), pos(1))
{
    auto s = tex.getSize();
    _spr.setTexture(tex);
    _spr.setOrigin(s.x*0.5f, s.y*0.5f);
    _spr.setPosition(_x, _y);
}


void Food::setPosition(const Vec2f& pos)
{
    _x = pos(0);
    _y = pos(1);
    _spr.setPosition(_x, _y);
}

void Food::render(sf::RenderTarget& target)
{
    target.draw(_spr);
}

Vec2f Food::getPos(void) const
{
    return Vec2f(_x, _y);
}
