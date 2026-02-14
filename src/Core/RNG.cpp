#include "Core/RNG.h"
#include <limits>

namespace Core {

RNG::RNG(uint32_t seed_value) : m_engine(seed_value) {}

void RNG::seed(uint32_t new_seed) {
    m_engine.seed(new_seed);
}

int RNG::range(int min, int max) {
    if (min > max) return min; 
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_engine);
}

float RNG::float01() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(m_engine);
}

} // namespace Core
