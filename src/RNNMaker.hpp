#ifndef RNN_MAKER_HPP
#define RNN_MAKER_HPP


#include <vector>
#include "RNN.hpp"


class RNNMaker {
public:
    RNNMaker(uint64_t nInputs, uint64_t nOutputs);

    void createCircularLayeredRNN(uint64_t nLayers, uint64_t nPerLayer);

    uint64_t addLayer(uint64_t nNeurons);

    //  connect layer id 1 fully as input for layer id 2
    void connectLayers(uint64_t layerId1, uint64_t layerId2, float weightRange = 10.0f);

    const RNN::Genome& getGenome(void) const;
    RNN getRNN(void) const;

private:
    class Layer {
    public:
        Layer(uint64_t lastNeuronId, uint64_t nNeurons);

        std::vector<uint64_t>   _neurons;
    };

    uint64_t                    _nInputs;
    uint64_t                    _nOutputs;
    uint64_t                    _lastNeuronId;

    RNN::Genome                 _genome;

    std::vector<Layer>          _layers;
};


#endif // RNN_MAKER_HPP
