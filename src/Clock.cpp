#include "Clock.hpp"

#include <iostream>


void Clock::begin(void) {
    begin_ = std::chrono::steady_clock::now();
}

double Clock::end(Clock::CastType cast) {
    end_ = std::chrono::steady_clock::now();

    switch (cast) {
        case SECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(end_-begin_);
            return diff.count();
        }
        break;

        case MILLISECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_-begin_);
            return diff.count();
        }
        break;

        case MICROSECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end_-begin_);
            return diff.count();
        }
        break;

        case NANOSECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end_-begin_);
            return diff.count();
        }
        break;
    }
}

double Clock::end(const std::string& caption, Clock::CastType cast) {
    end_ = std::chrono::steady_clock::now();

    switch (cast) {
        case SECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(end_-begin_);
            std::cout << caption << diff.count() << " s" << std::endl;
            return diff.count();
        }
        break;

        case MILLISECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_-begin_);
            std::cout << caption << diff.count() << " ms" << std::endl;
            return diff.count();
        }
        break;

        case MICROSECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end_-begin_);
            std::cout << caption << diff.count() << " us" << std::endl;
            return diff.count();
        }
        break;

        case NANOSECONDS:
        {
            auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end_-begin_);
            std::cout << caption << diff.count() << " ns" << std::endl;
            return diff.count();
        }
        break;
    }
}
