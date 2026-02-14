#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Entities {

using EntityID = uint32_t;

struct StatsComponent {
    int health;
    int max_health;
    int base_damage;
    float level;
};

struct InventoryComponent {
    int gold = 0;
    int potions = 0;
    int wood = 0;
    int stone = 0;
    int iron = 0;
    // For simplicity in this phase, we just store ItemIDs
    // A more complex system would have a list of Item objects or slots
    std::vector<std::string> items; // List of ItemIDs
    std::string equipped_weapon; // ItemID
    std::string equipped_armor;  // ItemID
    // For Phase 1, we keep simple counters. 
    // Phase 2 will introduce proper Item IDs.
};

// Simple aggregate for Phase 1 to replace "Character" class
struct GameEntity {
    EntityID id;
    std::string name;     // Character Name (e.g., "Hero")
    std::string username; // Login ID
    std::string password; // Login Password
    StatsComponent stats;
    InventoryComponent inventory;
    bool is_active = true;
};

} // namespace Entities
