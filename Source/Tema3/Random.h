#pragma once

#include "Easing.h"

#include <include/glm.h>

#include <random>

class Random {
    static inline std::random_device randomDevice;
    static inline std::mt19937 randomEngine { randomDevice() };
    static inline std::uniform_real_distribution<float> randomFloat { 0, std::nextafter(1.0f, 2.0f) };
    static inline std::uniform_int_distribution<int> randomInt { 0, (std::numeric_limits<int>::max)() };

public:
    static inline float Float(float start = 0, float end = 1) {
        using namespace Easing;

        return Lerp(start, (max)(start, end), randomFloat(randomEngine));
    }

    static inline int Int(int start = 0, int end = (std::numeric_limits<int>::max)()) {
        return start + randomInt(randomEngine) % (end - start + 1L);
    }

    Random() = delete;

    Random(Random const& other) = delete;
    Random& operator=(Random const& other) = delete;

    Random(Random&& other) = delete;
    Random& operator=(Random&& other) = delete;

    ~Random() = delete;
};
