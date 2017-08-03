#include "RNNRenderer.hpp"
#include "RNN.hpp"
#include "Random.hpp"

#include <map>


namespace {

    void checkLayers(std::map<uint64_t, uint64_t>& layers,
        const std::vector<RNN::Neuron>& neurons,
        uint64_t currentLayer, uint64_t currentNeuron)
    {
        for (auto& c : neurons[currentNeuron].conns) {
            if (layers.find(c.from) == layers.end()) {
                layers[c.from] = currentLayer+1;
                checkLayers(layers, neurons, currentLayer+1, c.from);
            }
        }
    }

}


RNNRenderer::RNNRenderer(
    const std::string& neuronSpriteFileName,
    uint64_t width, uint64_t height) :
    _rnn(nullptr),
    _width(width),
    _height(height),
    _connections(sf::Lines, 0)
{
    _neuronTex.loadFromFile(neuronSpriteFileName);
    auto s = _neuronTex.getSize();
    _neuronSpr.setTexture(_neuronTex);
    _neuronSpr.setOrigin(s.x/2, s.y/2);
}

void RNNRenderer::setRNN(const RNN* rnn)
{
    _rnn = rnn;
    auto& neurons = _rnn->getNeurons();

    auto nInputs = _rnn->getNumInputs();
    auto nOutputs = _rnn->getNumOutputs();

    std::map<uint64_t, uint64_t> layers;

    for (auto i=nInputs; i<nInputs+nOutputs; ++i) {
        layers[i] = 0;
        checkLayers(layers, neurons, 0, i);
    }

    std::map<uint64_t, uint64_t> nPerLayer;

    uint64_t nLayers = 0;
    for (auto& l : layers) {
        printf("Neuron %llu layer: %llu\n", l.first, l.second);
        if (l.second > nLayers)
            nLayers = l.second;
        nPerLayer[l.second]++;
    }
    ++nLayers;

    std::map<uint64_t, uint64_t> nPositioned;
    for (auto i=0llu; i<neurons.size(); ++i) {
        float a = -0.9/nLayers;
        a *= layers[i];
        a += 0.95;
        //auto nl = nPerLayer[layers[i]];
        _neuronPositions.emplace_back(_width*a, (rnd()%_height)*0.9+0.05*_height);
    }

    for (auto i=0llu; i<nInputs; ++i) {
        _neuronPositions[i].x = 0.05*_width;
    }

    for (auto i=0llu; i<neurons.size(); ++i) {
        for (auto& c : neurons[i].conns) {
            _connections.append(sf::Vertex(_neuronPositions[i]+sf::Vector2f(-16.0f, 0.0f), sf::Color(255, 255, 255)));
            _connections.append(sf::Vertex(_neuronPositions[c.from]+sf::Vector2f(16.0f, 0.0f), sf::Color(64, 64, 64)));
        }
    }
}

void RNNRenderer::render(sf::RenderTarget& renderTarget)
{
    auto& neurons = _rnn->getNeurons();
    auto ni = _rnn->getNumInputs();
    auto no = _rnn->getNumOutputs();

    renderTarget.draw(_connections);

    for (auto i=0llu; i<neurons.size(); ++i) {
        _neuronSpr.setPosition(_neuronPositions[i]);

        float c = neurons[i].a*255;

        if (i < ni)
            _neuronSpr.setColor(sf::Color(0, c, 0));
        else if (i < ni+no)
            _neuronSpr.setColor(sf::Color(c, 0, 0));
        else
            _neuronSpr.setColor(sf::Color(c, c, c));
        renderTarget.draw(_neuronSpr);
    }
}
