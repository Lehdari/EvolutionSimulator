#ifndef MUTATION_HPP
#define MUTATION_HPP


#include "RNN.hpp"


class Mutation {
public:
    enum Type {
        UNKNOWN             = 0,
        WEIGHT              = 1,
        ADDCONNECTION       = 2,
        REMOVECONNECTION    = 3
    };

    Mutation(const RNN::Genome& genome, uint64_t nNeurons);

    void apply(RNN::Genome& genome);

private:
    Type        _type;
    uint64_t    _geneId;

    uint64_t    _from;
    uint64_t    _to;
    float       _weight;
};


#endif // MUTATION_HPP
