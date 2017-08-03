#include "RNN.hpp"

#include <cstdio>


RNN::RNN(uint64_t nInputs, uint64_t nOutputs, const RNN::Genome& genome) :
    _nInputs(nInputs), _nOutputs(nOutputs)
{
    for (auto i=0llu; i<_nInputs; ++i)
        _neurons.emplace_back();

    for (auto i=0llu; i<_nOutputs; ++i)
        _neurons.emplace_back();

    for (auto& cg : genome) {
        uint64_t maxId = std::max(cg.from, cg.to);
        if (maxId >= _neurons.size())
            _neurons.resize(maxId+1);

        _neurons[cg.to].conns.emplace_back(cg.from, cg.weight);
    }
}

void RNN::setInput(uint64_t inputId, float val)
{
    if (inputId > _nInputs)
        return;

    _neurons[inputId].i = val;
}

float RNN::getOutput(uint64_t outputId) const
{
    if (outputId > _nOutputs)
        return 0.0f;

    return _neurons[_nInputs+outputId].a;
}

uint64_t RNN::getNumInputs(void) const
{
    return _nInputs;
}

uint64_t RNN::getNumOutputs(void) const
{
    return _nOutputs;
}

const std::vector<RNN::Neuron>& RNN::getNeurons(void) const
{
    return _neurons;
}

void RNN::tick(void)
{
    for (auto& n : _neurons) {
        for (auto& c : n.conns)
            n.i += _neurons[c.from].a*c.w;
    }
    for (auto& n : _neurons) {
        n.a = activationLogistic(n.i);
        n.i = 0.0f;
    }
}

void RNN::print(bool printConns) const
{
    for (auto i=0u; i<_neurons.size(); ++i) {
        auto& n = _neurons[i];
        printf("Neuron %u\ti: %0.3f\ta: %0.3f\n", i, n.i, n.a);
        if (printConns)
            for (auto& c : n.conns)
                printf("  Conn from neuron %llu\tw: %0.3f\n", c.from, c.w);
    }
}
