#pragma once
#include <map>
#include <string>

namespace Data {

struct ItemData {
    std::string name;
    int value; // Damage or Defense
    int cost;
    int wood_req;
    int stone_req;
    int iron_req;
};

struct WeaponData {
    std::string name;
    int damage;
    int cost;
    int wood_req;
    int stone_req;
    int iron_req;
};

struct ArmorData {
    std::string name;
    int defense; // Adds to max health in original logic
    int cost;
    int wood_req;
    int stone_req;
    int iron_req;
};

/**
 * @brief Temporary Hardcoded Database.
 * 
 * In Phase 2, this will load from JSON.
 */
class ItemDatabase {
public:
    static void Initialize();
    static const WeaponData* GetWeapon(const std::string& name);
    static const ArmorData* GetArmor(const std::string& name);
    static void LoadData(const std::string& filename);
    const static std::map<std::string, WeaponData>& GetWeapons() { return s_weapons; }
    const static std::map<std::string, ArmorData>& GetArmors() { return s_armors; }

private:
    static std::map<std::string, WeaponData> s_weapons;
    static std::map<std::string, ArmorData> s_armors;
};

} // namespace Data
