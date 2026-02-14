#include "Core/SaveManager.h"
#include "Core/Logger.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace Core {

bool SaveManager::Save(const Entities::GameEntity& player, const std::string& filename) {
    nlohmann::json j;
    
    // Serialize Player Data
    j["name"] = player.name;
    j["id"] = player.id;
    
    // Stats
    j["stats"]["max_health"] = player.stats.max_health;
    j["stats"]["health"] = player.stats.health;
    j["stats"]["base_damage"] = player.stats.base_damage;
    j["stats"]["level"] = player.stats.level;

    // Inventory
    j["inventory"]["gold"] = player.inventory.gold;
    j["inventory"]["potions"] = player.inventory.potions;
    j["inventory"]["wood"] = player.inventory.wood;
    j["inventory"]["stone"] = player.inventory.stone;
    j["inventory"]["iron"] = player.inventory.iron;
    j["inventory"]["items"] = player.inventory.items;
    j["inventory"]["equipped_weapon"] = player.inventory.equipped_weapon;
    j["inventory"]["equipped_armor"] = player.inventory.equipped_armor;
    j["inventory"]["items"] = player.inventory.items;

    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::Error("Failed to open save file for writing: " + filename);
        return false;
    }
    
    file << j.dump(4);
    Logger::Info("Game Saved Successfully.");
    return true;
}

bool SaveManager::Load(Entities::GameEntity& player, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::Error("Failed to open save file for reading: " + filename);
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        player.name = j["name"];
        player.id = j["id"];

        player.stats.max_health = j["stats"]["max_health"];
        player.stats.health = j["stats"]["health"];
        player.stats.base_damage = j["stats"]["base_damage"];
        player.stats.level = j["stats"]["level"];

        if (j.contains("inventory")) {
            player.inventory.gold = j["inventory"].value("gold", 0);
            player.inventory.potions = j["inventory"].value("potions", 0);
            player.inventory.wood = j["inventory"].value("wood", 0);
            player.inventory.stone = j["inventory"].value("stone", 0);
            player.inventory.iron = j["inventory"].value("iron", 0);
            // Equipped items might not exist in older saves, provide defaults or handle gracefully
            player.inventory.equipped_weapon = j["inventory"].value("equipped_weapon", "");
            player.inventory.equipped_armor = j["inventory"].value("equipped_armor", "");
            player.inventory.items = j["inventory"]["items"].get<std::vector<std::string>>();
        }

        Logger::Info("Game Loaded Successfully.");
        return true;
    } catch (const std::exception& e) {
        Logger::Error(std::string("Load Failed: ") + e.what());
        return false;
    }
}

} // namespace Core
