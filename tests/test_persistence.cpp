#include "Core/SaveManager.h"
#include <iostream>
#include <cassert>
#include <filesystem>

void TestPersistence() {
    std::cout << "Testing Save/Load System..." << std::endl;

    // 1. Setup Test Entity
    Entities::GameEntity original;
    original.id = 999;
    original.name = "TestSaveUser";
    original.stats = {150, 200, 25, 2.5f};
    original.inventory.gold = 500;
    original.inventory.items = {"TestItem1", "TestItem2"};
    original.inventory.equipped_weapon = "TestSword";
    original.inventory.equipped_armor = "TestArmor";

    std::string filename = "UserData/TestSaveUser.json";

    // 2. Save
    bool saved = Core::SaveManager::Save(original, filename);
    assert(saved);
    assert(std::filesystem::exists(filename));

    // 3. Load
    Entities::GameEntity loaded;
    bool loaded_success = Core::SaveManager::Load(loaded, filename);
    assert(loaded_success);

    // 4. Verify
    assert(loaded.name == original.name);
    assert(loaded.stats.max_health == original.stats.max_health);
    assert(loaded.stats.level == original.stats.level);
    assert(loaded.inventory.gold == original.inventory.gold);
    assert(loaded.inventory.items.size() == 2);
    assert(loaded.inventory.items[0] == "TestItem1");
    assert(loaded.inventory.equipped_weapon == "TestSword");

    std::cout << "Persistence Test PASSED." << std::endl;
    
    // Cleanup
    std::filesystem::remove(filename);
}

int main() {
    // Ensure UserData exists
    std::filesystem::create_directory("UserData");
    TestPersistence();
    return 0;
}
