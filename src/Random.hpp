#ifndef RANDOM_HPP
#define RANDOM_HPP


#define RNDF ((rnd()%100000)/100000.0f)
#define RNDRANGE(MIN, MAX) (MIN+rnd()%(MAX-MIN))


#include <random>


extern std::default_random_engine  rnd;


#endif // RANDOM_HPP
