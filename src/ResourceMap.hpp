#ifndef RESOURCE_MAP_HPP
#define RESOURCE_MAP_HPP


#include <SFML/Graphics.hpp>

#include "MathTypes.hpp"


class ResourceMap {
public:
    ResourceMap(const std::string& foodFertilityMapFileName,
                double width, double height);

    void render(sf::RenderTarget& target);

    Vec2f getRandomFoodPos(void) const;
    float getFoodFertility(const Vec2f& pos) const;

private:
    double  _width;
    double  _height;

    double  _imgWidthToWidth;
    double  _imgHeightToHeight;

    sf::Image   _foodFertilityMapImg;
    sf::Vector2u    _foodFertilityMapImgSize;

    sf::Texture _foodFertilityMapTex;
    sf::Sprite  _foodFertilityMapSpr;
};


#endif // RESOURCE_MAP_HPP
