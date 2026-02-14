#include "Domain/CombatService.h"
#include "Entities/Entity.h"
#include "Core/RNG.h"
#include "Core/ServiceLocator.h"
#include "Data/ItemDatabase.h"
#include <iostream>
#include <cassert>

void TestCombat() {
    std::cout << "Testing Combat..." << std::endl;
    // Setup
    auto rng = std::make_shared<Core::RNG>(12345);
    Core::ServiceLocator::Provide(rng);
    Data::ItemDatabase::Initialize();
    
    Entities::GameEntity player;
    player.name = "Hero";
    player.stats.health = 100;
    player.stats.max_health = 100;
    player.stats.base_damage = 10;
    player.stats.level = 1.0f;

    Entities::GameEntity enemy;
    enemy.name = "Goblin";
    enemy.stats.health = 50;
    enemy.stats.max_health = 50;
    enemy.stats.base_damage = 5;
    enemy.stats.level = 1.0f;
    
    Domain::CombatService service;
    service.Initialize();

    // Equip Weapon (Wooden Sword: +5 Damage)
    player.inventory.equipped_weapon = "Wooden Sword";

    std::cout << "Player Attack..." << std::endl;
    auto result = service.ExecuteTurn(player, enemy, "attack");
    
    // Check log exists
    assert(!result.log.empty());
    
    // Base(10) + Weapon(5) = 15.
    // Formula: (15 * 2) - Def(0) = 30 Damage.
    // Enemy HP: 50 - 30 = 20.
    std::cout << "Enemy Health: " << enemy.stats.health << std::endl;
    assert(enemy.stats.health == 20);
    
    std::cout << "Combat Test PASSED." << std::endl;
}

int main() {
    TestCombat();
    return 0;
}
