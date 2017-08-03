#ifndef RNN_HPP
#define RNN_HPP


#include <vector>
#include <cstdint>
#include <cmath>


class RNN {
public:
    struct ConnectionGene {
        uint64_t    from;
        uint64_t    to;
        float       weight;

        ConnectionGene(uint64_t from, uint64_t to, float weight) :
            from(from), to(to), weight(weight) {}
    };

    using Genome = std::vector<ConnectionGene>;

    struct Connection {
        uint64_t    from;   //  id of the neuron where the connection originates from
        float       w;      //  connection weight

        Connection(uint64_t from, float w) :
            from(from), w(w) {}
    };

    struct Neuron {
        float                   i = 0.0f;   //  input sum
        float                   a = 0.0f;   //  activation
        std::vector<Connection> conns;      //  connections from other neurons
    };

    RNN(uint64_t nInputs, uint64_t nOutputs, const Genome& genome);

    void setInput(uint64_t inputId, float val);
    float getOutput(uint64_t outputId) const;

    uint64_t getNumInputs(void) const;
    uint64_t getNumOutputs(void) const;
    const std::vector<Neuron>& getNeurons(void) const;

    void tick(void);

    void print(bool printConns = true) const;

private:
    uint64_t    _nInputs;
    uint64_t    _nOutputs;

    std::vector<Neuron> _neurons;


    inline float activationLogistic(float x) {
        return 1.0f/(1.0f+expf(-x));
    }
};


#endif // RNN_HPP
