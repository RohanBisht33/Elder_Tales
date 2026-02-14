#pragma once
#include <random>
#include <cstdint>

namespace Core {

/**
 * @brief Deterministic Random Number Generator Wrapper.
 * 
 * Enforces seeded randomness for gameplay logic.
 * BANS: std::rand(), global raw std::random_device usage in logic.
 */
class RNG {
public:
    explicit RNG(uint32_t seed_value);

    // Disable copying to prevent accidental state duplication issues
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;

    /**
     * @brief Generates a random integer in [min, max] inclusive.
     */
    int range(int min, int max);

    /**
     * @brief Generates a float in [0.0, 1.0].
     */
    float float01();

    /**
     * @brief Re-seed the generator.
     */
    void seed(uint32_t new_seed);

private:
    std::mt19937 m_engine;
};

} // namespace Core
