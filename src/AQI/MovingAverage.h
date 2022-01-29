#pragma once

#include <Arduino.h>

namespace AQI {
    template<typename T, size_t N>
    class MovingAverage {
    public:
        MovingAverage &operator()(T sample) {
            _total += sample;
            if (_numSamples < N)
                _samples[_numSamples++] = sample;
            else {
                auto index = ++_numSamples % N;
                _numSamples = N + index;
                T &oldest = _samples[index];
                _total -= oldest;
                oldest = sample;
            }
            return *this;
        }

        operator float() const { return _total / std::min(_numSamples, N); }

        bool hasReachCapacity() const { return _numSamples >= N; }

    private:
        T _samples[N];
        size_t _numSamples{0};
        T _total{0};
    };
}



