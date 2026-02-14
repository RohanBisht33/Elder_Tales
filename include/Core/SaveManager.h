#pragma once
#include "Entities/Entity.h"
#include <string>

namespace Core {

class SaveManager {
public:
    static bool Save(const Entities::GameEntity& player, const std::string& filename);
    static bool Load(Entities::GameEntity& player, const std::string& filename);
};

} // namespace Core
