#include "MovingAverage.h"

namespace AQI {
    template<typename T, size_t N>
    MovingAverage<T, N> &MovingAverage<T, N>::operator()(T sample) {
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
}