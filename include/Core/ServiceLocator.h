#pragma once
#include <memory>
#include <iostream>

#include "Core/RNG.h"
#include "Core/Logger.h"

namespace Core {

/**
 * @brief Simple Service Locator.
 * 
 * Provides global access to core services without singletons.
 * In a larger engine, this might be replaced by a Dependency Injection framework.
 */
class ServiceLocator {
public:
    static void Provide(std::shared_ptr<RNG> rng) { m_rng = rng; }
    static RNG& GetRNG() { return *m_rng; }

private:
    static std::shared_ptr<RNG> m_rng;
};

} // namespace Core
