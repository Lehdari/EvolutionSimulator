#ifndef RNN_RENDERER_HPP
#define RNN_RENDERER_HPP


#include <SFML/Graphics.hpp>


class RNN;


class RNNRenderer {
public:
    RNNRenderer(const std::string& neuronSpriteFileName,
                uint64_t width, uint64_t height);

    void setRNN(const RNN* rnn);

    void render(sf::RenderTarget& renderTarget);

private:
    const RNN*  _rnn;

    sf::Texture _neuronTex;
    sf::Sprite  _neuronSpr;

    uint64_t    _width;
    uint64_t    _height;
    sf::VertexArray             _connections;
    std::vector<sf::Vector2f>   _neuronPositions;
};


#endif // RNN_RENDERER_HPP
