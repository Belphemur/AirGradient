#pragma once

#include <Arduino.h>

namespace AQI {
    template<typename T, size_t N>
    class MovingAverage {
    public:
        MovingAverage &operator()(T sample);

        operator float() const { return _total / std::min(_numSamples, N); }

        bool hasReachCapacity() const { return _numSamples >= N; }

    private:
        T _samples[N];
        size_t _numSamples{0};
        T _total{0};
    };
}



