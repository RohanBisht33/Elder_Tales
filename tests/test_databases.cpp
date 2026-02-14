#include "Data/ItemDatabase.h"
#include "Data/EnemyDatabase.h"
#include <iostream>
#include <cassert>

void TestItemDatabase() {
    std::cout << "Testing ItemDatabase..." << std::endl;
    Data::ItemDatabase::Initialize();

    const auto* sword = Data::ItemDatabase::GetWeapon("Iron Sword");
    assert(sword != nullptr);
    assert(sword->damage == 15);
    assert(sword->cost == 100);

    const auto* armor = Data::ItemDatabase::GetArmor("Steel Chest");
    assert(armor != nullptr);
    assert(armor->defense == 800);

    std::cout << "ItemDatabase PASSED." << std::endl;
}

void TestEnemyDatabase() {
    std::cout << "Testing EnemyDatabase..." << std::endl;
    Data::EnemyDatabase::Initialize();

    const auto* goblin = Data::EnemyDatabase::GetEnemy("Goblin");
    assert(goblin != nullptr);
    assert(goblin->base_hp == 80);
    assert(goblin->xp_reward == 10);

    const auto* dragon = Data::EnemyDatabase::GetEnemy("Dragon");
    assert(dragon != nullptr);
    assert(dragon->level_mult > 2.0f);

    std::cout << "EnemyDatabase PASSED." << std::endl;
}

int main() {
    TestItemDatabase();
    TestEnemyDatabase();
    return 0;
}
