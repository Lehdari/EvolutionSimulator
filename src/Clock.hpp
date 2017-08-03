#ifndef CLOCK_HPP
#define CLOCK_HPP


#include <chrono>
#include <string>


class Clock {
public:
    enum CastType {
        SECONDS,
        MILLISECONDS,
        MICROSECONDS,
        NANOSECONDS
    };

    void begin(void);
    double end(CastType cast);
    double end(const std::string& caption, CastType cast);

private:
    std::chrono::time_point<std::chrono::steady_clock> begin_;
    std::chrono::time_point<std::chrono::steady_clock> end_;
};


#endif // CLOCK_HPP
