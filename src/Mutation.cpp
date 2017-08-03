#include "Mutation.hpp"
#include "MathTypes.hpp"
#include "Random.hpp"


Mutation::Mutation(const RNN::Genome& genome, uint64_t nNeurons) :
    _type(UNKNOWN),
    _geneId(0),
    _from(0),
    _to(0),
    _weight(0.0f)
{
    if (genome.size() == 0)
        _type = ADDCONNECTION;
    else {
        _geneId = rnd()%genome.size();
        switch (rnd()%3) {
        case 0:
            _type = WEIGHT;
            break;
        case 1:
            _type = ADDCONNECTION;
            break;
        case 2:
            _type = REMOVECONNECTION;
            break;
        default:
            break;
        }
    }

    switch (_type) {
    case WEIGHT:
        _weight = RNDF*2.0f;
        _weight *= _weight;
        break;
    case ADDCONNECTION:
        _geneId = 0;
        _from = rnd()%nNeurons;
        _to = rnd()%nNeurons;
        _weight = -5.0f + RNDF * 10.0f;
        break;
    default:
        break;
    }
}

void Mutation::apply(RNN::Genome& genome)
{
    static uint64_t printCounter = 1000;

    switch (_type) {
    case WEIGHT:
        //  TEMP
        if (--printCounter == 0) {
            printf("Applying weight mutation to connection from %llu to %llu: %0.3f -> %0.3f\n",
                genome[_geneId].from, genome[_geneId].to, genome[_geneId].weight,
                genome[_geneId].weight + _weight);
            printCounter = 1000;
        }
        genome[_geneId].weight += _weight;
        break;
    case ADDCONNECTION:
        //  TEMP
        if (--printCounter == 0) {
            printf("Applying add connection mutation from %llu to %llu: %0.3f, genome width: %llu\n",
                _from, _to, _weight, genome.size()+1);
            genome.emplace_back(_from, _to, _weight);
            printCounter = 1000;
        }
        break;
    case REMOVECONNECTION:
        //  TEMP
        if (--printCounter == 0) {
            printf("Applying remove connection mutation from %llu to %llu, genome width: %llu\n",
                genome[_geneId].from, genome[_geneId].to, genome.size()-1);
            genome.erase(genome.begin()+_geneId);
            printCounter = 1000;
        }
        break;
    default:
        break;
    }
}
