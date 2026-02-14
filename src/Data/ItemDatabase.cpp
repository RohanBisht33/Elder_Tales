#include "Data/ItemDatabase.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "Core/Logger.h"

namespace Data {

std::map<std::string, Data::WeaponData> ItemDatabase::s_weapons;
std::map<std::string, Data::ArmorData> ItemDatabase::s_armors;

void ItemDatabase::Initialize() {
    std::ifstream file("data/items.json");
    if (!file.is_open()) {
        Core::Logger::Error("Failed to open data/items.json");
        return;
    }

    nlohmann::json j;
    try {
        file >> j;
    
        // Load Weapons
        for (const auto& item : j["weapons"]) {
            std::string name = item["name"];
            int damage = item["damage"];
            int cost = item.value("cost", 0);
            int wood = item.value("recipe", nlohmann::json::object()).value("wood", 0);
            int stone = item.value("recipe", nlohmann::json::object()).value("stone", 0);
            int iron = item.value("recipe", nlohmann::json::object()).value("iron", 0);
            
            s_weapons[name] = {name, damage, cost, wood, stone, iron};
        }

        // Load Armors
        for (const auto& item : j["armors"]) {
            std::string name = item["name"];
            int defense = item["defense"];
            int cost = item.value("cost", 0);
            int wood = item.value("recipe", nlohmann::json::object()).value("wood", 0);
            int stone = item.value("recipe", nlohmann::json::object()).value("stone", 0);
            int iron = item.value("recipe", nlohmann::json::object()).value("iron", 0);

            s_armors[name] = {name, defense, cost, wood, stone, iron};
        }
        
        Core::Logger::Info("ItemDatabase Loaded Successfully.");

    } catch (const std::exception& e) {
        Core::Logger::Error(std::string("JSON Parse Error: ") + e.what());
    }
}

const WeaponData* ItemDatabase::GetWeapon(const std::string& name) {
    auto it = s_weapons.find(name);
    if (it != s_weapons.end()) return &it->second;
    return nullptr;
}

const ArmorData* ItemDatabase::GetArmor(const std::string& name) {
    auto it = s_armors.find(name);
    if (it != s_armors.end()) return &it->second;
    return nullptr;
}

} // namespace Data
