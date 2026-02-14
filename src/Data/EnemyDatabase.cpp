#include "Data/EnemyDatabase.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Core/Logger.h"

namespace Data {

std::map<std::string, EnemyData> EnemyDatabase::m_enemies;

void EnemyDatabase::Initialize() {
    std::ifstream file("data/enemies.json");
    if (!file.is_open()) {
        Core::Logger::Error("Failed to open data/enemies.json");
        return;
    }

    nlohmann::json j;
    try {
        file >> j;
        for (const auto& item : j["enemies"]) {
            EnemyData enemy;
            enemy.name = item["name"];
            enemy.base_hp = item["base_hp"];
            enemy.base_damage = item.value("base_damage", 5); // Default to 5 if missing
            enemy.level_mult = item["level_mult"];
            enemy.xp_reward = item["xp_reward"]; // Use .value() for optional fields if needed
            m_enemies[enemy.name] = enemy;
        }
        Core::Logger::Info("EnemyDatabase Loaded Successfully.");
    } catch (const std::exception& e) {
        Core::Logger::Error(std::string("JSON Parse Error: ") + e.what());
    }
}

const EnemyData* EnemyDatabase::GetEnemy(const std::string& name) {
    auto it = m_enemies.find(name);
    if (it != m_enemies.end()) return &it->second;
    return nullptr;
}

} // namespace Data
