#ifndef GLOBALS_HPP
#define GLOBALS_HPP


#include <cstdint>


namespace globals {

    //  setup constants
    constexpr uint64_t worldW = 8000;
    constexpr uint64_t worldH = 8000;
    constexpr const char* foodFertilityMapFileName = "res/foodfertilitymap02.png";

    constexpr uint64_t viewPortW = 1920;
    constexpr uint64_t viewPortH = 1080;

    constexpr uint64_t nCreatures = 1500;
    constexpr uint64_t minCreatureInitLife = 10;
    constexpr uint64_t maxCreatureInitLife = 300;
    constexpr uint64_t maxCreatureLife = 1200;

    constexpr uint64_t initFoodNumber = 1000;
    constexpr uint64_t maxFoodNumber = 5000;
    constexpr uint64_t minFoodLifeIncrease = 500;
    constexpr uint64_t maxFoodLifeIncrease = 550;

    constexpr float maxWhiskerLen = 300.0f;

    //  helper constants
    constexpr uint64_t quadTreeDepth = (int)log2(std::max(worldW, worldH)/maxWhiskerLen);

    constexpr uint64_t foodPerTick = (nCreatures / (maxFoodLifeIncrease*30)) + 1;
}


#endif // GLOBALS_HPP
