#ifndef EVOLVER_HPP
#define EVOLVER_HPP


#include "Creature.hpp"
#include "Mutation.hpp"


class Evolver {
public:
    Evolver(std::vector<RenderableCreature>& creatures);

    uint64_t replaceRNN(uint64_t id);

private:
    std::vector<RenderableCreature>&    _creatures;
};


#endif // EVOLVER_HPP
