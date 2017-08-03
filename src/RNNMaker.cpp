#include "RNNMaker.hpp"
#include "Random.hpp"

#include <ctime>


RNNMaker::RNNMaker(uint64_t nInputs, uint64_t nOutputs) :
    _nInputs        (nInputs),
    _nOutputs       (nOutputs),
    _lastNeuronId   (_nInputs+_nOutputs)
{
    _layers.emplace_back(0, nInputs);
    _layers.emplace_back(nInputs, nOutputs);
}

RNNMaker::Layer::Layer(uint64_t lastNeuronId, uint64_t nNeurons)
{
    for (auto i=0llu; i<nNeurons; ++i)
        _neurons.push_back(i+lastNeuronId);
}

void RNNMaker::createCircularLayeredRNN(uint64_t nLayers, uint64_t nPerLayer)
{
    auto firstLayerId = addLayer(nPerLayer);
    auto lastLayerId = firstLayerId;

    for (auto i=1llu; i<nLayers; ++i) {
        auto newLayerId = addLayer(nPerLayer);
        connectLayers(lastLayerId, newLayerId);
        lastLayerId = newLayerId;
    }
    connectLayers(lastLayerId, firstLayerId);

    connectLayers(firstLayerId, 1);
    connectLayers(0, firstLayerId);
}

uint64_t RNNMaker::addLayer(uint64_t nNeurons)
{
    _layers.emplace_back(_lastNeuronId, nNeurons);
    _lastNeuronId += nNeurons;
    return _layers.size()-1;
}

void RNNMaker::connectLayers(uint64_t layerId1, uint64_t layerId2, float weightRange)
{
    auto& neurons1 = _layers.at(layerId1)._neurons;
    auto& neurons2 = _layers.at(layerId2)._neurons;

    for (auto& n1 : neurons1)
        for (auto& n2 : neurons2)
            _genome.emplace_back(n1, n2, (RNDF*2.0f*weightRange)-weightRange);
}

const RNN::Genome& RNNMaker::getGenome(void) const
{
    return _genome;
}

RNN RNNMaker::getRNN(void) const
{
    return RNN(_nInputs, _nOutputs, _genome);
}
