#include "Core/Game.h"
#include "Core/SaveManager.h"
#include "Core/UserManager.h"
#include "Core/ServiceLocator.h"
#include "Core/Logger.h"
#include "Data/ItemDatabase.h"
#include "Data/EnemyDatabase.h"
#include "Domain/CombatService.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

namespace Core {

// Helper for input
std::string GetInput() {
    std::string line;
    std::cout << "> ";
    std::getline(std::cin, line);
    return line;
}

Game::Game() : m_running(true), m_state(GameState::Login) {
    // 1. Initialize Services
    m_rng = std::make_shared<RNG>(12345); // Fixed seed for now
    ServiceLocator::Provide(m_rng);
    
    Data::ItemDatabase::Initialize();
    Data::ItemDatabase::Initialize();
    Data::EnemyDatabase::Initialize();
    m_combatService.Initialize();

    m_player.id = 1;
    m_player.name = "Hero";
    m_player.stats = {200, 200, 10, 1.0f}; // 200 HP, Lvl 1
    m_player.inventory = {1000, 0, 0, 0, 1, {}, "Wooden Sword", "Wooden Chest"}; // Default Gear

    Logger::Info("Engine Initialized [Standard: AA]");
}

Game::~Game() {}

void Game::Run() {
    while (m_running) {
        switch (m_state) {
            case GameState::Login:      HandleLogin(); break;
            case GameState::MainMenu:   HandleMainMenu(); break;
            case GameState::Explore:    HandleExplore(); break;
            case GameState::Combat:     HandleCombat(); break;
            case GameState::Inventory:  HandleInventory(); break;
        case GameState::Shop:       HandleShop(); break;
        case GameState::GameOver:
            std::cout << "Returning to Main Menu..." << std::endl;
            m_state = GameState::MainMenu;
            // Maybe reset player HP?
            m_player.stats.health = m_player.stats.max_health;
            break;
        case GameState::Exit:       
                SaveManager::Save(m_player, "UserData/" + m_player.name + ".json");
                m_running = false; 
                break;
            default:                    m_state = GameState::MainMenu; break;
        }
    }
    Logger::Info("Game Shutting Down.");
}

void Game::HandleLogin() {
    // Ensure UserManager is initialized
    static bool userManagerInit = false;
    if (!userManagerInit) {
        Core::UserManager::Initialize();
        userManagerInit = true;
    }

    std::cout << "Welcome to Elder Tales (Refactored)." << std::endl;
    std::cout << "1. Login\n2. Register\n3. Quit" << std::endl;
    std::string choice = GetInput();
    
    if (choice == "1") {
        std::cout << "Enter Username: ";
        std::string username = GetInput();
        std::cout << "Enter Password: ";
        std::string password = GetInput();

        if (Core::UserManager::Login(username, password)) {
            // Attempt to load character data
            if (Core::SaveManager::Load(m_player, "UserData/" + username + ".json")) {
                m_player.username = username;
                m_player.password = password; // Keep in memory for session
                std::cout << "Login Successful. Welcome back, " << m_player.name << "!" << std::endl;
                m_state = GameState::MainMenu;
            } else {
                std::cout << "Login Successful, but no save data found. Creating new character." << std::endl;
                m_player.username = username;
                m_player.password = password;
                std::cout << "Enter Character Name: ";
                m_player.name = GetInput();
                // Reset stats for new character
                m_player.stats = {200, 200, 10, 1.0f};
                m_player.inventory = {1000, 0, 0, 0, 1, {}, "Wooden Sword", "Wooden Chest"};
                m_state = GameState::MainMenu;
            }
        } else {
            std::cout << "Invalid Username or Password." << std::endl;
        }

    } else if (choice == "2") {
        std::cout << "Enter Desired Username: ";
        std::string username = GetInput();
        if (Core::UserManager::UserExists(username)) {
            std::cout << "Username already taken." << std::endl;
            return;
        }

        std::cout << "Enter Desired Password: ";
        std::string password = GetInput();

        std::cout << "Enter Character Name: ";
        std::string charName = GetInput();

        if (Core::UserManager::Register(username, password)) {
             m_player.username = username;
             m_player.password = password;
             m_player.name = charName;
             m_player.stats = {200, 200, 10, 1.0f};
             m_player.inventory = {1000, 0, 0, 0, 1, {}, "Wooden Sword", "Wooden Chest"};
             
             // Initial Save
             Core::SaveManager::Save(m_player, "UserData/" + username + ".json");
             std::cout << "Registration Successful! You are logged in." << std::endl;
             m_state = GameState::MainMenu;
        } else {
            std::cout << "Registration Failed." << std::endl;
        }

    } else if (choice == "3") {
        m_state = GameState::Exit;
    } else if (choice == "server") {
        std::cout << "Registered Users (from database):" << std::endl;
        for (const auto& [user, pass] : Core::UserManager::GetUsers()) {
            std::cout << user << " " << pass << std::endl;
        }
    } else {
        std::cout << "Invalid Option." << std::endl;
    }
}

void ClearScreen() {
    // ANSI Escape Code to clear screen and move cursor to top-left
    std::cout << "\033[2J\033[1;1H";
}

void Game::AddLog(const std::string& message) {
    if (m_message_log.size() >= 5) { // Keep last 5 messages
        m_message_log.pop_front();
    }
    m_message_log.push_back(message);
}

void Game::HandleMainMenu() {
    ClearScreen();
    
    // Display Message Log
    if (!m_message_log.empty()) {
        std::cout << "--- Recent Events ---" << std::endl;
        for (const auto& msg : m_message_log) {
            std::cout << msg << std::endl;
        }
        std::cout << "---------------------" << std::endl;
    }

    std::cout << "\n[" << m_player.name << " Lvl " << m_player.stats.level << "]" << std::endl;
    std::cout << "HP: " << m_player.stats.health << "/" << m_player.stats.max_health << std::endl;
    std::cout << "Gold: " << m_player.inventory.gold << std::endl;
    std::cout << "Equipped: " << (m_player.inventory.equipped_weapon.empty() ? "None" : m_player.inventory.equipped_weapon) 
              << " / " << (m_player.inventory.equipped_armor.empty() ? "None" : m_player.inventory.equipped_armor) << std::endl;
    
    std::cout << "1. Explore\n2. Inventory\n3. Shop\n4. Craft\n5. Bank\n6. Rest\n7. Profile\n8. Help\n9. Potion\n10. Mining\n11. Fishing\n12. Tournament\n0. Quit" << std::endl;

    std::string choice = GetInput();
    if (choice == "1") m_state = GameState::Explore;
    else if (choice == "2") m_state = GameState::Inventory;
    else if (choice == "3") HandleShop();
    else if (choice == "4") HandleCraft();
    else if (choice == "5") HandleBank();
    else if (choice == "6") HandleRest();
    else if (choice == "7") HandleProfile();
    else if (choice == "8") HandleHelp();
    else if (choice == "9") HandlePotion();
    else if (choice == "10") HandleMining();
    else if (choice == "11") HandleFishing();
    else if (choice == "12") HandleTournament();
    else if (choice == "0") m_state = GameState::Exit;
    else if (choice == "cheat" || choice == "server") {
        // Admin commands hidden from menu but accessible
        if (choice == "cheat" || choice == "server") HandleAdmin();
    }
}

void Game::HandleRest() {
    std::cout << "\n" << m_player.name << " rests..." << std::endl;
    // Simple rest logic: Heal 25% Max HP
    int healAmount = m_player.stats.max_health / 4;
    m_player.stats.health += healAmount;
    if (m_player.stats.health > m_player.stats.max_health) {
        m_player.stats.health = m_player.stats.max_health;
    }
    std::cout << "Recovered " << healAmount << " HP." << std::endl;
}

void Game::HandleProfile() {
    std::cout << "\n--- PROFILE ---" << std::endl;
    std::cout << "Name: " << m_player.name << std::endl;
    std::cout << "Username: " << m_player.username << std::endl;
    std::cout << "Password: " << m_player.password << " (Visible as per legacy behavior)" << std::endl;
    std::cout << "Level: " << m_player.stats.level << std::endl;
    std::cout << "Base Damage: " << m_player.stats.base_damage << std::endl;
}

void Game::HandleHelp() {
    std::cout << "\n--- HELP ---" << std::endl;
    std::cout << "Explore: Search for enemies to fight." << std::endl;
    std::cout << "Inventory: View your items." << std::endl;
    std::cout << "Shop: Buy items and resources." << std::endl;
    std::cout << "Craft: Create items from resources." << std::endl;
    std::cout << "Bank: View all your assets." << std::endl;
    std::cout << "Rest: Heal 25% of your HP." << std::endl;
    std::cout << "Potion: Heal 33% of your HP (Requires Potion)." << std::endl;
    std::cout << "Profile: View your account details." << std::endl;
    std::cout << "Quit: Save and Exit." << std::endl;
}

void Game::HandleShop() {
    std::cout << "\n--- SHOP ---" << std::endl;
    std::cout << "Gold: " << m_player.inventory.gold << std::endl;
    std::cout << "1. Potion (500g)\n2. Wood (4g)\n3. Stone (8g)\n4. Iron (6g)\n0. Exit" << std::endl;
    std::cout << "Buy: ";
    std::string choice = GetInput();
    
    int cost = 0;
    std::string item = "";
    // int amount = 1; // Unused for now

    if (choice == "1") { item = "potion"; cost = 500; }
    else if (choice == "2") { item = "wood"; cost = 4; }
    else if (choice == "3") { item = "stone"; cost = 8; }
    else if (choice == "4") { item = "iron"; cost = 6; }
    else return;

    if (m_player.inventory.gold >= cost) {
        m_player.inventory.gold -= cost;
        if (item == "potion") m_player.inventory.potions++;
        else if (item == "wood") m_player.inventory.wood++;
        else if (item == "stone") m_player.inventory.stone++;
        else if (item == "iron") m_player.inventory.iron++;
        std::cout << "Bought " << item << "." << std::endl;
    } else {
        std::cout << "Not enough gold!" << std::endl;
    }
}

void Game::HandleCraft() {
    std::cout << "\n--- CRAFTING ---" << std::endl;
    // Simplified: List all craftable weapons
    int idx = 1;
    std::vector<std::string> options;
    for (const auto& [name, data] : Data::ItemDatabase::GetWeapons()) {
        std::cout << idx << ". " << name << " (W:" << data.wood_req << " S:" << data.stone_req << " I:" << data.iron_req << ")" << std::endl;
        options.push_back(name);
        idx++;
    }
    std::cout << "0. Exit" << std::endl;
    
    std::string input = GetInput();
    int choice = 0;
    try { choice = std::stoi(input); } catch (...) { return; }
    
    if (choice > 0 && choice <= static_cast<int>(options.size())) {
        std::string target = options[choice-1];
        const auto& recipe = Data::ItemDatabase::GetWeapons().at(target);
        
        if (m_player.inventory.wood >= recipe.wood_req && 
            m_player.inventory.stone >= recipe.stone_req && 
            m_player.inventory.iron >= recipe.iron_req) {
            
            m_player.inventory.wood -= recipe.wood_req;
            m_player.inventory.stone -= recipe.stone_req;
            m_player.inventory.iron -= recipe.iron_req;
            m_player.inventory.items.push_back(target);
            std::cout << "Crafted " << target << "!" << std::endl;
        } else {
            std::cout << "Not enough resources." << std::endl;
        }
    }
}

void Game::HandleBank() {
    std::cout << "\n--- BANK ---" << std::endl;
    std::cout << "Gold: " << m_player.inventory.gold << std::endl;
    std::cout << "Potions: " << m_player.inventory.potions << std::endl;
    std::cout << "Wood: " << m_player.inventory.wood << std::endl;
    std::cout << "Stone: " << m_player.inventory.stone << std::endl;
    std::cout << "Iron: " << m_player.inventory.iron << std::endl;
}

void Game::HandlePotion() {
    if (m_player.inventory.potions > 0) {
        m_player.inventory.potions--;
        int heal = m_player.stats.max_health / 3;
        m_player.stats.health += heal;
        if (m_player.stats.health > m_player.stats.max_health) m_player.stats.health = m_player.stats.max_health;
        std::cout << "Used Potion. Health: " << m_player.stats.health << "/" << m_player.stats.max_health << std::endl;
    } else {
        std::cout << "No potions!" << std::endl;
    }
}

void Game::HandleAdmin() {
    std::cout << "Admin Command: ";
    std::string cmd = GetInput();
    if (cmd == "cheat") {
        m_player.inventory.gold += 10000;
        m_player.inventory.wood += 1000;
        m_player.inventory.stone += 1000;
        m_player.inventory.iron += 1000;
        std::cout << "Resources added." << std::endl;
    } else if (cmd == "server") {
        std::cout << "Registered Users (from database):" << std::endl;
        std::ifstream f("userdata.txt");
        if (f.is_open()) {
            std::string line;
            while(getline(f, line)) std::cout << line << std::endl;
        }
    }
}

void Game::HandleMining() {
    std::cout << "Mining..." << std::endl;
    // Simple RNG: chance to get Stone or Iron
    int roll = Core::ServiceLocator::GetRNG().range(1, 100);
    if (roll > 50) {
        int amount = Core::ServiceLocator::GetRNG().range(1, 5);
        if (roll > 80) {
            m_player.inventory.iron += amount;
            std::cout << "You found " << amount << " Iron!" << std::endl;
        } else {
            m_player.inventory.stone += amount;
            std::cout << "You found " << amount << " Stone!" << std::endl;
        }
    } else {
        std::cout << "You found nothing." << std::endl;
    }
}

void Game::HandleFishing() {
    std::cout << "Fishing..." << std::endl;
    // Simple RNG
    int roll = Core::ServiceLocator::GetRNG().range(1, 100);
    if (roll > 60) {
        std::cout << "You caught a fish! (Healed 10 HP)" << std::endl;
        m_player.stats.health = std::min(m_player.stats.max_health, m_player.stats.health + 10);
    } else {
        std::cout << "No bite." << std::endl;
    }
}

void Game::HandleTournament() {
    std::cout << "\n--- TOURNAMENT ---" << std::endl;
    std::cout << "Enter the Gauntlet? (y/n): ";
    std::string input = GetInput();
    if (input != "y") return;

    // Sequential battles
    int wins = 0;
    while (m_player.stats.health > 0) {
        Entities::GameEntity enemy;
        enemy.name = "Gladiator " + std::to_string(wins + 1);
        enemy.stats.max_health = 50 + (wins * 20);
        enemy.stats.health = enemy.stats.max_health;
        enemy.stats.base_damage = 5 + (wins * 2);
        enemy.stats.level = m_player.stats.level; // Scaling

        std::cout << "Fighting " << enemy.name << "..." << std::endl;
        
        m_current_enemy = enemy;
        // HandleCombat loop logic inline or reuse state?
        // Reusing m_state = Combat would return to main menu after one fight.
        // So we must run the loop here manually or push a stack.
        // For simplicity: Run a mini-loop here.
        
        bool in_combat = true;
        while (in_combat && m_player.stats.health > 0) {
             std::cout << "Your HP: " << m_player.stats.health << " | Enemy HP: " << m_current_enemy.stats.health << std::endl;
             std::cout << "1. Attack\n2. Flee" << std::endl;
             std::string cmd = GetInput();
             if (cmd == "1") {
                 auto result = m_combatService.ExecuteTurn(m_player, m_current_enemy, "attack");
                 for(const auto& l : result.log) std::cout << l << std::endl;
                 if (result.enemy_died) { in_combat = false; wins++; std::cout << "Victory!" << std::endl; }
             } else {
                 in_combat = false; // Fleeing ends tournament
                 std::cout << "You fled the tournament." << std::endl;
                 return;
             }
        }
        
        if (m_player.stats.health <= 0) {
             std::cout << "You were defeated in round " << (wins + 1) << "." << std::endl;
             // Handle death (Game Over state or just respawn at menu with 1 hp?)
             // Consistent with normal combat: Game Over.
             m_state = GameState::GameOver;
             return; // Exit tournament
        }

        std::cout << "Continue to round " << (wins + 1) << "? (y/n): ";
        if (GetInput() != "y") break;
    }
    std::cout << "Tournament ended. Wins: " << wins << std::endl;
}

void Game::HandleInventory() {
    std::cout << "\n--- INVENTORY ---" << std::endl;
    if (m_player.inventory.items.empty()) {
        std::cout << "Empty." << std::endl;
    } else {
        for (size_t i = 0; i < m_player.inventory.items.size(); ++i) {
            std::cout << (i + 1) << ". " << m_player.inventory.items[i] << std::endl;
        }
    }
    std::cout << "0. Back" << std::endl;
    
    // For Phase 4 POC, simple back navigation. 
    // Equip logic would require mapping index to item type (Weapon/Armor) via Database.
    std::string input = GetInput();
    if (input == "0") {
        m_state = GameState::MainMenu;
        return;
    }

    try {
        int index = std::stoi(input) - 1;
        if (index >= 0 && index < static_cast<int>(m_player.inventory.items.size())) {
            std::string itemName = m_player.inventory.items[index];
            
            // Check if Weapon
            const auto* weapon = Data::ItemDatabase::GetWeapon(itemName);
            if (weapon) {
                m_player.inventory.equipped_weapon = itemName;
                std::cout << "Equipped " << itemName << "!" << std::endl;
                return;
            }

            // Check if Armor
            const auto* armor = Data::ItemDatabase::GetArmor(itemName);
            if (armor) {
                 m_player.inventory.equipped_armor = itemName;
                 std::cout << "Equipped " << itemName << "!" << std::endl;
                 return;
            }

            std::cout << "Cannot equip " << itemName << "." << std::endl;
        } else {
            std::cout << "Invalid selection." << std::endl;
        }
    } catch (...) {
        std::cout << "Invalid input." << std::endl;
    }
}

void Game::HandleExplore() {
    Logger::Info("Exploring...");
    
    // 50% chance of combat
    if (m_rng->range(0, 1) == 1) {
        
        // Spawn Enemy from Database
        // For Phase 2 Demo, we pick a random one or just "Goblin" to verify JSON loaded
        // Let's try to get "Orc" to see if JSON works (Goblin was hardcoded before too)
        
        std::vector<std::string> possible_enemies = {
            "Goblin", "Orc", "Slime", "Wolf", "Bear", "Bandit", "Skeleton", "Troll"
        };
        std::string enemy_name = possible_enemies[m_rng->range(0, static_cast<int>(possible_enemies.size()) - 1)];
        
        const auto* enemyData = Data::EnemyDatabase::GetEnemy(enemy_name);
        
        if (enemyData) {
            AddLog("Encountered " + enemy_name + "!");
            m_current_enemy.id = 2;
            m_current_enemy.name = enemyData->name;
            m_current_enemy.stats.max_health = static_cast<int>(static_cast<float>(enemyData->base_hp) * enemyData->level_mult); // Simple logic
            m_current_enemy.stats.health = m_current_enemy.stats.max_health;
            m_current_enemy.stats.base_damage = enemyData->base_damage;
            m_current_enemy.stats.level = 1.0f;
            m_state = GameState::Combat;
        } else {
             Logger::Error("Failed to find enemy data for " + enemy_name);
             m_state = GameState::MainMenu;
        }

    } else {
        AddLog("You explored but found nothing.");
        m_state = GameState::MainMenu;
    }
}

void Game::HandleCombat() {
    if (m_current_enemy.stats.health <= 0) {
        AddLog("You defeated " + m_current_enemy.name + "!");
        
        // Reward Logic?
        // AddLog("You gained 10 XP.");
        
        m_state = GameState::MainMenu;
        return;
    }

    std::cout << "\n--- Combat vs " << m_current_enemy.name << " (HP: " << m_current_enemy.stats.health << ") ---" << std::endl;
    std::cout << "Your HP: " << m_player.stats.health << "/" << m_player.stats.max_health << std::endl;
    std::cout << "1. Attack\n2. Flee" << std::endl; // Basic menu

    std::string input = GetInput();
    
    if (input == "1") {
        auto result = m_combatService.ExecuteTurn(m_player, m_current_enemy, "attack");
        
        for (const auto& log : result.log) {
            std::cout << log << std::endl;
        }

        if (result.enemy_died) {
            std::cout << "You have defeated " << m_current_enemy.name << "!" << std::endl;
            m_player.inventory.gold += 100; // Reward
            m_state = GameState::MainMenu;
        }
        else if (result.player_died) {
            std::cout << "You have been defeated..." << std::endl;
            m_state = GameState::GameOver;
        }
    } else if (input == "2") {
        if (Core::ServiceLocator::GetRNG().range(0, 1) == 0) {
            std::cout << "You fled!" << std::endl;
            m_state = GameState::MainMenu;
        } else {
            std::cout << "Failed to flee!" << std::endl;
            auto result = m_combatService.ExecuteTurn(m_player, m_current_enemy, "skip"); // Player turn skipped
             for (const auto& log : result.log) {
                std::cout << log << std::endl;
            }
             if (result.player_died) {
                std::cout << "You have been defeated..." << std::endl;
                m_state = GameState::GameOver;
            }
        }
    }
}

} // namespace Core
