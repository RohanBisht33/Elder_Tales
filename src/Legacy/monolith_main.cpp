#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <random>
#include <tuple>

using namespace std;

// RNG Helper
int get_random(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// --- Q-Learning AI Components ---

struct AIState {
    int playerHpPercent; // 0-10 scale
    int aiHpPercent;     // 0-10 scale

    bool operator<(const AIState& other) const {
        return tie(playerHpPercent, aiHpPercent) < tie(other.playerHpPercent, other.aiHpPercent);
    }
};

enum class AIAction { ATTACK, HEAL };

class QLearningAgent {
private:
    map<pair<AIState, AIAction>, double> qTable;
    double alpha = 0.1;  // Learning rate
    double gamma = 0.9;  // Discount factor
    double epsilon = 0.1; // Exploration rate

public:
    QLearningAgent() {}

    AIAction chooseAction(const AIState& state) {
        // Epsilon-greedy strategy
        if (get_random(0, 100) / 100.0 < epsilon) {
            return static_cast<AIAction>(get_random(0, 1)); // Random action
        }
        else {
            // Exploit: choose best action
            double maxQ = -1e9;
            AIAction bestAction = AIAction::ATTACK;

            // Check Q-values for all actions
            double qAttack = qTable[{state, AIAction::ATTACK}];
            double qHeal = qTable[{state, AIAction::HEAL}];

            // If Q-values are 0 (unexplored), default might be attack, but let's check
            if (qTable.find({state, AIAction::ATTACK}) == qTable.end() && 
                qTable.find({state, AIAction::HEAL}) == qTable.end()) {
                 return AIAction::ATTACK; // Default
            }

            if (qAttack >= qHeal) return AIAction::ATTACK;
            else return AIAction::HEAL;
        }
    }

    void updateQTable(const AIState& state, AIAction action, const AIState& nextState, int reward) {
        double currentQ = qTable[{state, action}];
        double maxFutureQ = max(qTable[{nextState, AIAction::ATTACK}], qTable[{nextState, AIAction::HEAL}]);
        qTable[{state, action}] = currentQ + alpha * (reward + gamma * maxFutureQ - currentQ);
    }

    void train(int episodes = 10000) {
        // Simulate simplified battles to populate Q-Table
        for (int i = 0; i < episodes; ++i) {
            int hpPlayer = 100;
            int hpAI = 100;

            while (hpPlayer > 0 && hpAI > 0) {
                // Determine current state bucket
                AIState state = { hpPlayer / 10, hpAI / 10 };
                
                // Choose action
                AIAction action = chooseAction(state);

                // Perform action (Simulation)
                int reward = 0;
                if (action == AIAction::ATTACK) {
                    hpPlayer -= 10; // Assume avg damage
                    reward = 10;
                } else {
                    hpAI = min(100, hpAI + 20); // Assume heavy heal
                    reward = -5; // Cost of not attacking
                }

                // Opponent turn (Player attacks back simplified)
                // If AI is low, player likely tries to finish, if AI is high, player attacks normal
                hpAI -= 8; 

                // Terminal rewards
                if (hpPlayer <= 0) reward += 50;
                if (hpAI <= 0) reward -= 50;

                // Next state
                AIState nextState = { max(0, hpPlayer) / 10, max(0, hpAI) / 10 };

                updateQTable(state, action, nextState, reward);
                
                // Update internal simulation state
                hpPlayer = max(0, hpPlayer);
                hpAI = max(0, hpAI);
            }
        }
        cout << "AI Training Completed (" << episodes << " episodes)." << endl;
    }
};

// Global AI Agent
QLearningAgent globalAI;

// --- End AI Components ---

class Player; // Forward declaration

class Weapon {
public:
    int base_damage;
    int level_requirement;
    int gold;
    int iron;
    int stone;
    int wood;
    int amount;


    Weapon() : amount(0), base_damage(0), level_requirement(0), gold(0), iron(0), stone(0), wood(0) {}
    Weapon(int ginti, int dmg, int lvl_req, int cost, int loha, int patthar, int lakdi) : amount(ginti), base_damage(dmg), level_requirement(lvl_req), gold(cost), iron(loha), stone(patthar), wood(lakdi) {}
};

class Armor {
public:
    int base_health;
    int level_requirement;
    int gold;
    int iron;
    int stone;
    int wood;
    int amount;


    Armor() : amount(0), base_health(0), level_requirement(0), gold(0), iron(0), stone(0), wood(0) {}
    Armor(int ginti, int sehat, int lvl_req, int cost, int loha, int patthar, int lakdi) : amount(ginti), base_health(sehat), level_requirement(lvl_req), gold(cost), iron(loha), stone(patthar), wood(lakdi) {}
};

class Character {
public:
    string name;
    float level;
    int health;
    int health_max;
    int damage;
    int potion;
    int gold;
    int wood;
    int stone;
    int iron;
    bool login;
    Weapon* equipped_weapon; // Initialize to nullptr
    string equipped_sword;
    Armor* equipped_armor;
    string equipped_chest;

    Character() : name(""), level(0.1f), health(200), health_max(200), damage(0), potion(0), gold(0), wood(0), stone(0), iron(0), login(false), equipped_weapon(nullptr), equipped_armor(nullptr) {}
    virtual ~Character() {}

    bool do_damage(Character& enemy) {
        // Calculate weapon damage
        int weapon_damage = equipped_weapon ? equipped_weapon->base_damage : 0;

        // Calculate base damage: Scale it with player's level
        int base_damage = weapon_damage + static_cast<int>(level * 10 * 5) + get_random(0, 10); // Base damage is influenced by level and randomness

        // Add variance to make damage a bit unpredictable
        int variance = get_random(0, 5); // Variance between 0 and 5
        int total_damage = base_damage + variance;

        // Adjust damage based on enemy's health
        total_damage += enemy.health_max / 100; // Extra damage proportional to enemy's health

        // Check for evasion
        if (get_random(0, 99) < 20) { // 20% chance to evade
            cout << "Mr." << enemy.name << " evades " << name << "-san's attack." << endl;
            total_damage = 0;
        }
        else {
            cout << name << "-san hurts Mr." << enemy.name << "!" << endl;
            cout << "Damage by player: " << total_damage << endl;
        }

        // Apply damage to the enemy
        enemy.health -= total_damage;
        return enemy.health <= 0;
    }

    // Renamed to be more descriptive and used by AI
    bool perform_attack(Character& target) {
        cout << "Mr." << name << "'s health: " << health << "/" << health_max << endl;

        // Calculate base damage: Scale it with player's level
        int base_damage = static_cast<int>(level * 10 * 5) + get_random(0, 10); 

        // Add variance 
        int variance = get_random(0, 5); 
        int total_damage = base_damage + variance;

        // Adjust damage based on enemy's health
        total_damage += health_max / 100; 

        // Check for evasion
        if (get_random(0, 99) < 20) { 
            cout << target.name << "-san evades Mr." << name << "'s attack." << endl;
            total_damage = 0;
        }
        else {
            cout << name << "-san hurts Mr." << target.name << "!" << endl;
            cout << "Damage by enemy: " << total_damage << endl;
        }

        // Apply damage
        target.health -= total_damage;
        return target.health <= 0;
    }

    void heal_self() {
         int heal_amount = health_max * 0.25; // Heal 25% percent
         health += heal_amount;
         if (health > health_max) health = health_max;
         cout << "Mr." << name << " heals themselves for " << heal_amount << " HP!" << endl;
         cout << "Mr." << name << "'s health: " << health << "/" << health_max << endl;
    }
};

class Enemy : public Character {
public:
    Enemy(Character& player) {
        // Ensure level is at least 0.1
        float playerLevel = player.level;
        level = playerLevel > 0 ? playerLevel : 1.0f;

        // Create a list of possible enemies for different level ranges
        vector<string> enemyNames;
        if (level * 10 < 5) {
            enemyNames = { "Goblin", "Orc", "Slime" };
        }
        else if (level * 10 < 10) {
            enemyNames = { "Majin", "Kajin", "Goblin Emperor" };
        }
        else if (level * 10 < 15) {
            enemyNames = { "Kajin", "Goblin Emperor", "Demon Knight", "High Orc" };
        }
        else if (level * 10 < 20) {
            enemyNames = { "Goblin Emperor", "Holy Emperor", "Demon Emperor" };
        }
        else if (level * 10 < 25) {
            enemyNames = { "Demon Emperor", "Black Dragon", "Dark Elf", "Invincible Man" };
        }
        else if (level * 10 > 24) {
            enemyNames = { "Black Dragon", "Dark Elf", "Ancient Lich","Vampire Queen", "Elder Knight" };
        }

        // Randomly select an enemy type from the list
        if (!enemyNames.empty()) {
            name = enemyNames[get_random(0, enemyNames.size() - 1)];
        } else {
             name = "Unknown Monster";
        }

        // Assign health based on the enemy type and player (Simplified mapping for brevity, logic preserved)
        int base_hp_factor = (player.health / 2 + 1); 
        
        // Use helper for random offset
        int rand_offset = get_random(0, base_hp_factor > 0 ? base_hp_factor : 1);

        if (name == "Slime") health_max = (rand_offset + 50) * static_cast<int>(level);
        else if (name == "Goblin") health_max = (rand_offset + 80) * static_cast<int>(level);
        else if (name == "Orc") health_max = (rand_offset + 100) * static_cast<int>(level);
        else if (name == "Majin") health_max = (rand_offset + 130) * static_cast<int>(level);
        else if (name == "Kajin") health_max = (rand_offset + 150) * static_cast<int>(level);
        else if (name == "Goblin Emperor") health_max = (rand_offset + 170) * static_cast<int>(level);
        else if (name == "Demon Knight") health_max = (rand_offset + 180) * static_cast<int>(level);
        else if (name == "High Orc") health_max = (rand_offset + 200) * static_cast<int>(level);
        else if (name == "Demon Emperor") health_max = (rand_offset + 220) * static_cast<int>(level);
        else if (name == "Holy Emperor") health_max = (rand_offset + 240) * static_cast<int>(level);
        else if (name == "Black Dragon") health_max = (rand_offset + 260) * static_cast<int>(level);
        else if (name == "Invincible Man") health_max = (rand_offset + 300) * static_cast<int>(level);
        else if (name == "Dark Elf") health_max = (rand_offset + 330) * static_cast<int>(level);
        else if (name == "Ancient Lich") health_max = (rand_offset + 350) * static_cast<int>(level);
        else if (name == "Vampire Queen") health_max = (rand_offset + 400) * static_cast<int>(level);
        else if (name == "Elder Knight") health_max = (rand_offset + 450) * static_cast<int>(level);
        else health_max = (rand_offset + 50) * static_cast<int>(level);

        // Initialize current health to maximum health
        health = health_max;
    }

    // AI Turn Logic
    void take_turn(Character& player) {
        // Calculate bucket state (0-10)
        int playerHpBucket = (player.health * 10) / (player.health_max > 0 ? player.health_max : 1);
        int aiHpBucket = (health * 10) / (health_max > 0 ? health_max : 1);
        
        // Clamp buckets
        if (playerHpBucket < 0) playerHpBucket = 0;
        if (playerHpBucket > 10) playerHpBucket = 10;
        if (aiHpBucket < 0) aiHpBucket = 0;
        if (aiHpBucket > 10) aiHpBucket = 10;

        AIState state = { playerHpBucket, aiHpBucket };
        AIAction action = globalAI.chooseAction(state);

        if (action == AIAction::ATTACK) {
             perform_attack(player);
        } else if (action == AIAction::HEAL) {
             heal_self();
        }
    }
};

class Player : public Character {
public:
    string state;
    Enemy* enemy = nullptr;
    map<string, void(Player::*)()> commands;
    map<string, int> items; // Map to hold the quantity of each item
    map<string, int> item_prices; // Map to hold the price of each item
    map<string, string> userNames; // username to password
    map<string, Weapon> weapons;
    map<string, Armor> armors;

    string username;
    string password;

    Player() {
        // Initialize commands map
        commands["quit"] = &Player::quit;
        commands["get"] = &Player::get;
        commands["status"] = &Player::status;
        commands["rest"] = &Player::rest;
        commands["explore"] = &Player::explore;
        commands["flee"] = &Player::flee;
        commands["attack"] = &Player::attack;
        commands["bank"] = &Player::bank;
        commands["potion"] = &Player::drink_potion;
        commands["bundle"] = &Player::bundle;
        commands["buy"] = &Player::buy; 
        commands["register"] = &Player::Signup;
        commands["login"] = &Player::Login;
        commands["server"] = &Player::Server;
        commands["profile"] = &Player::Profile;
        commands["equip"] = &Player::equip;
        commands["craft"] = &Player::craft;
        commands["cheat"] = &Player::cheat;

        //weapons
        weapons["Wooden Sword"] = Weapon(0, 5, 1, 10, 50, 100, 250);
        weapons["Iron Sword"] = Weapon(0, 15, 3, 100, 250, 100, 50);
        weapons["Steel Sword"] = Weapon(0, 30, 5, 1000, 150, 150, 50);
        weapons["Mythril Sword"] = Weapon(0, 50, 8, 2000, 200, 50, 150);
        weapons["Adamantite Sword"] = Weapon(0, 75, 10, 5000, 50, 250, 100);
        weapons["Diamond Sword"] = Weapon(0, 100, 15, 10000, 750, 500, 1000);

        //armors
        armors["Wooden Chest"] = Armor(0, 200, 1, 10, 50, 100, 250);
        armors["Iron Chest"] = Armor(0, 500, 3, 100, 250, 100, 50);
        armors["Steel Chest"] = Armor(0, 800, 5, 1000, 150, 150, 50);
        armors["Mythril Chest"] = Armor(0, 1000, 8, 2000, 200, 50, 150);
        armors["Adamantite Chest"] = Armor(0, 1250, 10, 5000, 50, 250, 100);
        armors["Diamond Chest"] = Armor(0, 1500, 15, 10000, 750, 500, 1000);

        // Initialize items and prices
        item_prices["potion"] = 500;
        item_prices["wood"] = 4;
        item_prices["stone"] = 8;
        item_prices["iron"] = 6;

        Reset(); // Initialize default values
    }

    ~Player() {
        if (enemy) {
            delete enemy;
            enemy = nullptr;
        }
    }

    void craft() {
        int count1 = 1;
        for (const auto& sword : weapons) {
            cout << count1 << ". " << sword.first << ":\t\t" << sword.second.gold << " gold required\t" << sword.second.iron << "Iron required\t" << sword.second.stone << " Stone required\t" << sword.second.wood << " Wood required" << endl;
            count1 += 1;
        }
        int count2 = 1;
        cout << endl << endl;
        for (const auto& chest : armors) {
            cout << count2 << ". " << chest.first << ":\t\t" << chest.second.gold << " gold required\t" << chest.second.iron << "Iron required\t" << chest.second.stone << " Stone required\t" << chest.second.wood << " Wood required" << endl;
            count2 += 1;
        }
        cout << "Enter the weapon you want to craft (eg. Wooden Sword or Wooden Chest): ";
        string item;
        getline(cin, item);
        if (weapons.find(item) != weapons.end()) {
            if (iron >= weapons[item].iron and stone >= weapons[item].stone and wood >= weapons[item].wood) {
                weapons[item].amount += 1;
                cout << item << " is crafted successfully!" << endl;
                Save(username);
            }
            else {
                cout << "Not enough resources to craft " << item << "." << endl;
            }
        }
        else if (armors.find(item) != armors.end()) {
            if (iron >= armors[item].iron and stone >= armors[item].stone and wood >= armors[item].wood) {
                armors[item].amount += 1;
                cout << item << " is crafted successfully!" << endl;
                Save(username);
            }
            else {
                cout << "Not enough resources to craft " << item << "." << endl;
            }
        }
        else {
            cout << "This item does not exist." << endl;
        }
    }

    void equip() {
        for (const auto& weapon : weapons) {
            if (weapon.second.amount >= 1) {
                cout << weapon.first << ":\t \t \tLevel " << weapon.second.level_requirement << " required!\t \t \tDamage:\t \t \t" << weapon.second.base_damage << ":\t \t \tOwned:\t \t \t" << weapon.second.amount << endl;
            }
        }
        for (const auto& armor : armors) {
            if (armor.second.amount >= 1) {
                cout << armor.first << ":\t \t \tLevel " << armor.second.level_requirement << " required!\t \t \tHealth:\t \t \t" << armor.second.base_health << ":\t \t \tOwned:\t \t \t" << armor.second.amount << endl;
            }
        }

        string item_name;
        bool isitem = false;
        for (const auto& weapon : weapons) {
            if (weapon.second.amount > 0) { isitem = true; break; }
        }
        if (!isitem) {
             for (const auto& armor : armors) {
                if (armor.second.amount > 0) { isitem = true; break; }
            }
        }

        if (isitem) {
            cout << "Enter the item you want to equip: ";
            getline(cin, item_name);

            if (weapons.find(item_name) != weapons.end()) {
                 if (weapons[item_name].amount > 0) {
                    if (level * 10 >= weapons[item_name].level_requirement) {
                        equipped_weapon = &weapons[item_name];
                        equipped_sword = item_name;
                        Save(username);
                        cout << "Equipped " << item_name << "." << endl;
                    }
                    else {
                        cout << "Level too low to equip " << item_name << "." << endl;
                    }
                 } else {
                     cout << "You don't have " << item_name << "." << endl;
                 }
            }
            else if (armors.find(item_name) != armors.end()) {
                 if (armors[item_name].amount > 0) {
                    if (level * 10 >= armors[item_name].level_requirement) {
                        equipped_armor = &armors[item_name];
                        equipped_chest = item_name;
                        health_max = static_cast<int>(2000 * level) + armors[equipped_chest].base_health;
                        health = health_max;
                        Save(username);
                        cout << "Equipped " << item_name << "." << endl;
                    }
                    else {
                        cout << "Level too low to equip " << item_name << "." << endl;
                    }
                 } else {
                     cout << "You don't have " << item_name << "." << endl;
                 }
            }
            else {
                cout << item_name << " does not exist" << endl;
            }
        }
        else {
            cout << "You don't have any items to equip." << endl;
        }
    }


    void Reset() {
        state = "normal";
        level = 0.1f;
        health = 200;
        health_max = 200;
        potion = 1;
        gold = 1000;
        wood = 1000;
        stone = 500;
        iron = 750;
        equipped_weapon = nullptr;
        equipped_sword = "unknown sword";
        equipped_armor = nullptr;
        equipped_chest = "unknown chest";
        login = false;
        
        if (enemy) {
            delete enemy;
            enemy = nullptr;
        }

        for (auto& sword : weapons) {
            sword.second.amount = 0;
        }
        for (auto& chest : armors) {
            chest.second.amount = 0;
        }
    }

    void quit() {
        cout << "Type 'register' if you are new or Type 'login' to enter: ";
        Reset();
    }

    void get() {
        cout << "Available commands: " << endl;
        for (const auto& command : commands) {
            if (command.first != "bundle" and command.first != "server") {
                cout << command.first << endl;
            }
        }
        cout << endl;
    }

    void status() {
        cout << name << "-san's health: " << health << "/" << health_max << endl << "Level is: " << level * 10 << endl;
        if (equipped_weapon != nullptr) {
            cout << name << "-san's equipped weapon: " << equipped_sword << endl;
        }
        if (equipped_armor != nullptr) {
            cout << name << "-san's equipped armor: " << equipped_chest << endl;
        }
        if (equipped_weapon == nullptr && equipped_armor == nullptr) {
            cout << "No weapon or armor is equipped " << endl;
        }
    }

    void tired() {
        cout << name << "-san feels tired." << endl;
        health = max(1, health - 10);
        Save(username);
    }
    void drink_potion() {
        if (potion > 0) {
            potion -= 1;
            health += health_max / 3;
            if (health > health_max) {
                health = health_max;
            }
            status();
        }
        else if (health >= health_max) {
            cout << "Health full already!";
            health = health_max;
        }
        else {
            cout << "Not enough potions, please buy more!!";
        }
        Save(username);
    }

    void buy() {
        if (state != "normal") {
            cout << "Can't buy during a fight!" << endl;
            return;
        }

        cout << "Your gold: " << gold << endl << "Available items for purchase: " << endl;
        for (const auto& item : item_prices) {
            cout << item.first << ":\t\t" << item.second << " gold each" << endl;
        }
        // Add weapons to the shop
        cout << "Available weapons for purchase: " << endl;

        for (const auto& weapon : weapons) {
            cout << weapon.first << ":\t \t \t" << weapon.second.gold << " gold (Level " << weapon.second.level_requirement << " required)" << endl;
        }

        cout << "Available armors for purchase: " << endl;

        for (const auto& armor : armors) {
            cout << armor.first << ":\t \t \t" << armor.second.gold << " gold (Level " << armor.second.level_requirement << " required)" << endl;
        }

        cout << "Enter the item and quantity you want to buy (e.g., 'potion 3' or 'Wooden Sword 1'), or type 'cancel' to exit: ";

        string input;
        getline(cin, input);

        if (input == "cancel") {
            cout << "Purchase cancelled." << endl;
            return;
        }

        string item;
        int quantity;

        // Find the position of the last space
        size_t last_space_pos = input.find_last_of(' ');
        if (last_space_pos != string::npos) {
            item = input.substr(0, last_space_pos); // Item name is everything before the last space
            try {
                quantity = stoi(input.substr(last_space_pos + 1)); // Quantity is the number after the last space
            }
            catch (...) {
                cout << "Invalid quantity entered." << endl;
                return;
            }
        }
        else {
            cout << "Invalid input format!" << endl;
            return;
        }

        cout << "Item: " << item << ", Quantity: " << quantity << endl;

        int total_cost;
        if (item_prices.find(item) != item_prices.end()) {
            total_cost = item_prices[item] * quantity;
            if (gold >= total_cost) {
                // Specialized handling for resource items
                if (item == "potion") {
                     potion += quantity;
                } else if (item == "wood") {
                    wood += quantity;
                } else if (item == "stone") {
                    stone += quantity;
                } else if (item == "iron") {
                    iron += quantity;
                } else {
                     // Generic item handling not really supported by current inventory system
                }

                gold -= total_cost;
                cout << "You bought " << quantity << " " << item << "(s) for " << total_cost << " gold." << endl;
                cout << "Remaining gold: " << gold << endl;
                Save(username);
            }
            else {
                cout << "Not enough gold to buy " << quantity << " " << item << "(s)." << endl;
            }
        }
        else if (weapons.find(item) != weapons.end()) {
            total_cost = quantity * weapons[item].gold;
            if (gold >= total_cost) {
                weapons[item].amount += quantity;
                gold -= total_cost;
                cout << "You bought " << quantity << " " << item << "(s) for " << total_cost << " gold." << endl;
                cout << "Remaining gold: " << gold << endl;
                Save(username);
            }
            else {
                cout << "Not enough gold to buy " << quantity << " " << item << "(s)." << endl;
            }
        }
        else if (armors.find(item) != armors.end()) {
            total_cost = quantity * armors[item].gold;
            if (gold >= total_cost) {
                armors[item].amount += quantity;
                gold -= total_cost;
                cout << "You bought " << quantity << " " << item << "(s) for " << total_cost << " gold." << endl;
                cout << "Remaining gold: " << gold << endl;
                Save(username);
            }
            else {
                cout << "Not enough gold to buy " << quantity << " " << item << "(s)." << endl;
            }
        } else {
             cout << "Item does not exist!" << endl;
        }
    }

    void bundle() {
        potion += 10;
        gold += 50000;
        level += 2;
        if (equipped_armor) {
            health_max = static_cast<int>(2000 * level) + armors[equipped_chest].base_health;
        } else {
            health_max = static_cast<int>(2000 * level);
        }
        health = health_max;
        Save(username);
    }

    void bank() {
        // Initialize items map for display
        items["potion"] = potion;
        items["gold"] = gold;
        items["wood"] = wood;
        items["stone"] = stone;
        items["iron"] = iron;

        cout << "Items\t \t \tQuantity" << endl;
        for (const auto& item : items) {
            cout << item.first << "\t \t \t" << item.second << endl;
        }

        cout << endl << "Owned weapons:" << endl;
        for (const auto& weapon : weapons) {
            if (weapon.second.amount >= 1) {
                cout << weapon.first << ":\t \t \tDamage:\t \t \t" << weapon.second.base_damage << ":\t \t \tAmount:\t \t \t" << weapon.second.amount << endl;
            }
        }

        cout << endl << "Owned armors:" << endl;
        for (const auto& armor : armors) {
            if (armor.second.amount >= 1) {
                cout << armor.first << ":\t \t \tHealth:\t \t \t" << armor.second.base_health << ":\t \t \tAmount:\t \t \t" << armor.second.amount << endl;
            }
        }
        cout << endl;
    }

    void rest() {
        if (state != "normal") {
            cout << name << "-san can't rest now!" << endl;
            enemy_attacks();
        }
        else {
            // Replace rand() % 2
            if (get_random(0, 1)) {
                if (enemy) delete enemy;
                enemy = new Enemy(*this);
                cout << name << "-san is rudely awakened by Mr." << enemy->name << "!" << endl;
                state = "fight";
                enemy_attacks();
            }
            else {
                cout << name << "-san rests." << endl;
                if (health < health_max)
                    health = health_max;
                else {
                    cout << name << "-san slept too much." << endl;
                    health -= 10;
                }
            }
        }
        Save(username);
    }

    void explore() {
        if (state != "normal") {
            cout << name << "-san is too busy right now!" << endl;
            enemy_attacks();
        }
        else {
            // Replace rand() % 2
            if (get_random(0, 1)) {
                 if (enemy) delete enemy;
                enemy = new Enemy(*this);
                cout << name << "-san encounters a " << enemy->name << "!" << endl;
                state = "fight";
            }
            else {
                cout << name << "-san explores a twisty passage." << endl;
            }
        }
        Save(username);
    }

    void flee() {
        if (state != "fight") {
            cout << name << "-san runs in circles for a while." << endl;
            tired();
        }
        else {
            // Replace rand() % (health + 5) > rand() % (enemy + 1)
            // Note: enemy cannot be null if state is fight (but let's be safe)
            int enemy_hp = enemy ? enemy->health : 0;
            if (get_random(0, health + 4) > get_random(0, enemy_hp)) {
                cout << name << "-san flees from Mr." << (enemy ? enemy->name : "nobody") << "." << endl;
                if (enemy) {
                    delete enemy;
                    enemy = nullptr;
                }
                state = "normal";
            }
            else {
                cout << name << "-san couldn't escape from Mr." << (enemy ? enemy->name : "nobody") << "!" << endl;
                enemy_attacks();
            }
        }
        Save(username);
    }

    void attack() {
        if (state != "fight" || enemy == nullptr) {
            cout << name << "-san swats the air, without notable results." << endl;
            tired();
        }
        else {
            if (do_damage(*enemy)) {
                cout << name << "-san executes Mr." << enemy->name << "!" << endl;
                if (enemy) {
                    delete enemy;
                    enemy = nullptr;
                }
                state = "normal";
                
                // Reward logic
                if (get_random(0, health) < health_max) {
                    level += 0.1f;
                    health_max = static_cast<int>(2000 * level);
                     if (equipped_armor) {
                        health_max += armors[equipped_chest].base_health;
                    }

                    gold += 700;
                    cout << name << "-san feels stronger!" << endl;
                }
            }
            else {
                enemy_attacks();
            }
        }
        Save(username);
    }

    void enemy_attacks() {
        if (enemy) {
            enemy->take_turn(*this); // AI smart turn
           
            if (health <= 0) {
                 cout << name << "-san was slaughtered by Mr." << enemy->name << "!!!\nR.I.P." << endl;
                 health = 0;
            }
        }
    }
    void Server() {
        int count = 1;
        for (const auto& userEntry : userNames) {
            cout << count << ". " << "Username: " << userEntry.first << " , " << "Password: " << userEntry.second << endl;
            count++;
        }
    }
    void Profile() {
        cout << "Username: " << username << endl;
        cout << "Password: " << password << endl;
    }

     void Login() {
        if (login) {
            cout << "Already logged in as " << username << "!" << endl;
            return;
        }

        string enteredPassword;

        cout << "You can type 'cancel' to stop" << endl;
        cout << "Enter your username: ";
        cin >> username;
        if (username == "cancel") {
            quit();
            return;
        }

        while (userNames.find(username) == userNames.end()) {
            cout << "Username not found. Please register first." << endl;
            cout << "You can type 'cancel' to stop " << endl;
            cout << "Enter your username: ";
            cin >> username;
            if (username == "cancel") {
                quit();
                return;
            }
        }

        cout << "Enter your password: ";
        cin >> enteredPassword;

        while (userNames[username] != enteredPassword) {
            cout << "Incorrect password. Please try again." << endl;
            cout << "Enter your password: ";
            cin >> enteredPassword;
        }

        login = true;
        cout << "(type 'get' to get a list of actions)\n";
        Load(username);
        cout << name << "-san enters a dark cave, searching for adventure." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    }

    void Signup() {
        if (login) {
            cout << "Already logged in as " << username << "!" << endl;
            return;
        }

        cout << "What is your character's name? ";
        cin >> name;

        while (true) {
            cout << "Enter a username: ";
            cin >> username;

            if (userNames.find(username) != userNames.end()) {
                cout << "Username is unavailable, please try again.\n";
            }
            else {
                break;
            }
        }

        cout << "Enter a password: ";
        cin >> password;

        userNames[username] = password;
        login = true;
        equipped_weapon = nullptr;
        equipped_armor = nullptr;
        cout << "(type 'get' to get a list of actions)\n";
        cout << name << "-san enters a dark cave, searching for adventure." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        Save(username);
        SaveUserData();
    }

    void Save(const string& user) {
        string directory = "UserData/";
        string filepath = directory + user + "_savegame.txt";

        ofstream saveFile(filepath);
        if (!saveFile) {
            cout << "Error opening file for saving!" << endl;
            return;
        }

        saveFile << name << " " << username << " " << password << " " << level << " " << health << " " << health_max << " " << potion << " " << gold << " " << wood << " " << stone << " " << iron << " " << equipped_sword << " " << equipped_chest << endl;

        for (const auto& pair : weapons) {
            if (pair.second.base_damage != 0) {
                saveFile << pair.first << " " << pair.second.amount << " " << pair.second.base_damage << " " << pair.second.level_requirement << " " << pair.second.gold << " " << pair.second.iron << " " << pair.second.stone << " " << pair.second.wood << endl; 
            }
        }
        for (const auto& pair : armors) {
            if (pair.second.base_health != 0) {
                saveFile << pair.first << " " << pair.second.amount << " " << pair.second.base_health << " " << pair.second.level_requirement << " " << pair.second.gold << " " << pair.second.iron << " " << pair.second.stone << " " << pair.second.wood << endl;
            }
        }
        saveFile.close();
    }

    void Load(const string& user) {
        string directory = "UserData/";
        string filepath = directory + user + "_savegame.txt";
        ifstream loadFile(filepath);
        if (!loadFile) {
            cout << "No saved game data found for " << user << ". Starting new game." << endl;
            return;
        }

        string naam, uid, pass, chest, chest2, sword, sword2;
        float levl;
        int sehat, sehat2, drink, sona, lakdi, ston, loha;
        
        // Reading header
        while (loadFile >> naam >> uid >> pass >> levl >> sehat >> sehat2 >> drink >> sona >> lakdi >> ston >> loha >> sword >> sword2 >> chest >> chest2)
        {
            name = naam;
            username = uid;
            password = pass;
            level = levl;
            health = sehat;
            health_max = sehat2;
            potion = drink;
            gold = sona;
            wood = lakdi;
            stone = ston;
            iron = loha;
            equipped_sword = sword + " " + sword2;
            equipped_chest = chest + " " + chest2;
            break;
        }
        
        // Set pointers based on strings
        if (weapons.find(equipped_sword) != weapons.end()) {
             equipped_weapon = &weapons[equipped_sword];
        } else {
             equipped_weapon = nullptr;
        }
        
        if (armors.find(equipped_chest) != armors.end()) {
             equipped_armor = &armors[equipped_chest];
        } else {
             equipped_armor = nullptr;
        }

        // Read inventory
        string name, name2;
        int base, lvl, gold, iron, stone, wood, count;
        while (loadFile >> name >> name2 >> count >> base >> lvl >> gold >> iron >> stone >> wood) {
            string fullName = name + " " + name2;
            if (weapons.find(fullName) != weapons.end()) {
                weapons[fullName] = Weapon(count, base, lvl, gold, iron, stone, wood);
            }
            if (armors.find(fullName) != armors.end()) {
                armors[fullName] = Armor(count, base, lvl, gold, iron, stone, wood);
            }
        }

        loadFile.close();
    }

    void SaveUserData() {
        ofstream userFile("userdata.txt");
        if (!userFile) {
            cout << "Error opening file for saving usernames!" << endl;
            return;
        }
        for (const auto& pair : userNames) {
            userFile << pair.first << " " << pair.second << endl;
        }
        userFile.close();
    }

    void LoadUserData() {
        ifstream userFile("userdata.txt");
        if (!userFile) {
            return;
        }
        userNames.clear();
        string loadedUsername, loadedPassword;
        while (userFile >> loadedUsername >> loadedPassword) {
            userNames[loadedUsername] = loadedPassword;
        }
        userFile.close();
    }
    void cheat() {
        level = 20;
        gold = 1000000;
        iron = 1000000;
        stone = 1000000;
        wood = 1000000;
    }

};

int main() {
    // Basic AI Training at startup
    cout << "Training AI Opponent..." << endl;
    globalAI.train();

    Player p;
    p.LoadUserData();
    p.Reset();

    cout << "Type 'register' if you are new or Type 'login' to enter: ";

    string line;
    while (p.health > 0) {
        cout << "> ";
        getline(cin, line);
        bool commandFound = false;

        for (const auto& command : p.commands) {
            if (line.find(command.first) == 0) {
                 if (!p.login and (line == "register" or line == "login")) {
                    (p.*(command.second))();
                    commandFound = true;
                    break;
                 } else if (p.login and line != "register" and line != "login") {
                    (p.*(command.second))();
                    commandFound = true;
                    break;
                 }
            }
        }
        
        if (!commandFound) {
             if (p.login) {
                 cout << p.name << "-san doesn't understand the suggestion." << endl;
             } else {
                 cout << "Type 'register' if you are new or Type 'login' to enter: ";
             }
        }
    }
    return 0;
}