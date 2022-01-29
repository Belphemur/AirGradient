#include "MovingAverage.h"

namespace AQI {
    template<typename T, size_t N>
    MovingAverage<T, N> &MovingAverage<T, N>::operator()(T sample) {
        _total += sample;
        if (_numSamples < N)
            _samples[_numSamples++] = sample;
        else {
            _numSamples = N + (++_numSamples % N);
            T &oldest = _samples[_numSamples];
            _total -= oldest;
            oldest = sample;
        }
        return *this;
    }
}