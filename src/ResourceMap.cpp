#include "ResourceMap.hpp"
#include "Random.hpp"


ResourceMap::ResourceMap(const std::string& foodFertilityMapFileName,
    double width, double height) :
    _width  (width),
    _height (height)
{
    _foodFertilityMapImg.loadFromFile(foodFertilityMapFileName);
    _foodFertilityMapImgSize = _foodFertilityMapImg.getSize();

    _imgWidthToWidth = _foodFertilityMapImgSize.x / _width;
    _imgHeightToHeight = _foodFertilityMapImgSize.y / _height;

    _foodFertilityMapTex.loadFromImage(_foodFertilityMapImg);

    _foodFertilityMapSpr.setTexture(_foodFertilityMapTex);
    _foodFertilityMapSpr.setScale(
        _width / _foodFertilityMapImgSize.x,
        _height / _foodFertilityMapImgSize.y
    );
}

void ResourceMap::render(sf::RenderTarget& target)
{
    target.draw(_foodFertilityMapSpr);
}

Vec2f ResourceMap::getRandomFoodPos(void) const
{
    Vec2f pos(RNDF*_width, RNDF*_height);
    float f = getFoodFertility(pos);
    while (RNDF > f) {
        pos(0) = RNDF*_width;
        pos(1) = RNDF*_height;
        f = getFoodFertility(pos);
    }
    return pos;
}

float ResourceMap::getFoodFertility(const Vec2f& pos) const
{
    uint64_t x = pos(0)*_imgWidthToWidth;
    if (x >= _foodFertilityMapImgSize.x)
        x = _foodFertilityMapImgSize.x-1;
    uint64_t y = pos(1)*_imgHeightToHeight;
    if (y >= _foodFertilityMapImgSize.y)
        y = _foodFertilityMapImgSize.y-1;

    return _foodFertilityMapImg.getPixel(x, y).r*0.00392156863;
}
