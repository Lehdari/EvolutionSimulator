#include "Evolver.hpp"
#include "Random.hpp"


Evolver::Evolver(std::vector<RenderableCreature>& creatures) :
    _creatures(creatures)
{
}

uint64_t Evolver::replaceRNN(uint64_t id)
{
    uint64_t parentId = rnd() % _creatures.size();
    while (parentId == id)
        parentId = rnd() % _creatures.size();

    //  TEMP
    //printf("Replacing creature %llu using creature %llu as parent...\n", id, parentId);

    auto& parent = _creatures[parentId];

    //  genome
    auto genome = parent._genome;
    Mutation mutation(genome, parent._nNeurons);
    mutation.apply(genome);

    _creatures[id]._rnn = RNN(parent._rnn.getNumInputs(), parent._rnn.getNumOutputs(), genome),
    _creatures[id]._genome = genome;

    //  color
    _creatures[id]._r = parent._r;
    _creatures[id]._g = parent._g;
    _creatures[id]._b = parent._b;
    _creatures[id].applyColorMutation();

    return parentId;
}
