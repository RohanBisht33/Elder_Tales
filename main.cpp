#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>

using namespace std;

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

    Character() : name(""), level(0.1), health(200), health_max(200), equipped_weapon(nullptr), equipped_armor(nullptr) {}

    bool do_damage(Character& enemy) {
        // Calculate weapon damage
        int weapon_damage = equipped_weapon ? equipped_weapon->base_damage : 0;

        // Calculate base damage: Scale it with player's level
        int base_damage = weapon_damage + (level * 10 * 5) + (rand() % 11); // Base damage is influenced by level and randomness

        // Add variance to make damage a bit unpredictable
        int variance = rand() % 6; // Variance between 0 and 5
        int total_damage = base_damage + variance;

        // Adjust damage based on enemy's health
        total_damage += enemy.health_max / 100; // Extra damage proportional to enemy's health

        // Check for evasion
        if (rand() % 100 < 20) { // 20% chance to evade
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

    bool do_damage_2(Character& self) {
        cout << "Mr." << name << "'s health: " << health << "/" << health_max << endl;
        // Calculate weapon damage
//        int weapon_damage = equipped_weapon ? equipped_weapon->base_damage : 0;

        // Calculate base damage: Scale it with player's level
        int base_damage = (level * 10 * 5) + (rand() % 11); // Base damage is influenced by level and randomness

        // Add variance to make damage a bit unpredictable
        int variance = rand() % 6; // Variance between 0 and 5
        int total_damage = base_damage + variance;

        // Adjust damage based on enemy's health
        total_damage += health_max / 100; // Extra damage proportional to enemy's health

        // Check for evasion
        if (rand() % 100 < 20) { // 20% chance to evade
            cout << self.name << "-san evades Mr." << name << "'s attack." << endl;
            total_damage = 0;
        }
        else {
            cout << name << "-san hurts Mr." << self.name << "!" << endl;
            cout << "Damage by enemy: " << total_damage << endl;
        }

        // Apply damage
        self.health -= total_damage;
        return self.health <= 0;
    }
};

class Enemy : public Character {
public:
    Enemy(Character& player) {
        // Ensure level is at least 0.1
        float playerLevel = player.level;
        level = playerLevel > 0 ? playerLevel : 1;

        // Create a list of possible enemies for different level ranges
        vector<string> enemyNames;
        if (level < 5) {
            enemyNames = { "Goblin", "Orc", "Slime" };
        }
        else if (level * 10 < 10) {
            enemyNames = { "Majin", "Kajin", "Goblin Emperor" };
        }
        else if (level * 10 < 15) {
            enemyNames = { "Kajin", "Goblin Emperor", "Demon Knight, High Orc" };
        }
        else if (level * 10 < 20) {
            enemyNames = { "Goblin Emperor", "Holy Emperor", "Demon Emperor" };
        }
        else if (level * 10 < 25) {
            enemyNames = { "Demon Emperor", "Black Dragon", "Dark Elf", "Invincible Man" };
        }
        else if (level * 10 > 24) {
            enemyNames = { "Black Dragon", "Dark Elf", "Ancient Lich","Vampire Queen, Elder Knight" };
        }

        // Randomly select an enemy type from the list
        name = enemyNames[rand() % enemyNames.size()];

        // Assign health based on the enemy type and player
        if (name == "Slime") {
            health_max = (rand() % (player.health / 2 + 1) + 50) * level;
        }
        else if (name == "Goblin") {
            health_max = (rand() % (player.health / 2 + 1) + 80) * level;
        }
        else if (name == "Orc") {
            health_max = (rand() % (player.health / 2 + 1) + 100) * level;
        }
        else if (name == "Majin") {
            health_max = (rand() % (player.health / 2 + 1) + 130) * level;
        }
        else if (name == "Kajin") {
            health_max = (rand() % (player.health / 2 + 1) + 150) * level;
        }
        else if (name == "Goblin Emperor") {
            health_max = (rand() % (player.health / 2 + 1) + 170) * level;
        }
        else if (name == "Demon Knight") {
            health_max = (rand() % (player.health / 2 + 1) + 180) * level;
        }
        else if (name == "High Orc") {
            health_max = (rand() % (player.health / 2 + 1) + 200) * level;
        }
        else if (name == "Demon Emperor") {
            health_max = (rand() % (player.health / 2 + 1) + 220) * level;
        }
        else if (name == "Holy Emperor") {
            health_max = (rand() % (player.health / 2 + 1) + 240) * level;
        }
        else if (name == "Black Dragon") {
            health_max = (rand() % (player.health / 2 + 1) + 260) * level;
        }
        else if (name == "Invincible Man") {
            health_max = (rand() % (player.health / 2 + 1) + 300) * level;
        }
        else if (name == "Dark Elf") {
            health_max = (rand() % (player.health / 2 + 1) + 330) * level;
        }
        else if (name == "Ancient Lich") {
            health_max = (rand() % (player.health / 2 + 1) + 350) * level;
        }
        else if (name == "Vampire Queen") {
            health_max = (rand() % (player.health / 2 + 1) + 400) * level;
        }
        else if (name == "Elder Knight") {
            health_max = (rand() % (player.health / 2 + 1) + 450) * level;
        }


        // Initialize current health to maximum health
        health = health_max;
    }
};

class Player : public Character {
public:
    string state;
    Enemy* enemy = nullptr;
    map<string, void(Player::*)()> commands;
    map<string, int> items; // Map to hold the quantity of each item
    map<string, int> item_prices; // Map to hold the price of each item
    map<string, string> userName;
    // Replace the existing userNames declaration
    map<string, string> userNames; // username to password
    map<string, Weapon> weapons;
    map<string, Armor> armors;

    string username;
    string password;

    Player() {
        state = "normal";

        level = 0.1;
        health = 200;
        health_max = 200;
        potion = 1;
        gold = 1000;
        wood = 1000;
        stone = 500;
        iron = 750;
        login = false;
        equipped_weapon = nullptr;
        equipped_sword = "";
        equipped_armor = nullptr;
        equipped_chest = "";

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
        commands["buy"] = &Player::buy; // Updated to new buy function
        commands["register"] = &Player::Signup;
        commands["login"] = &Player::Login;
        commands["server"] = &Player::Server;
        commands["profile"] = &Player::Profile;
        commands["buy"] = &Player::buy; // Updated to new buy function
        commands["equip"] = &Player::equip;
        commands["craft"] = &Player::craft;

        //weapons
        weapons["WoodenSword"] = Weapon(0, 5, 1, 10, 50, 100, 250);
        weapons["IronSword"] = Weapon(0, 15, 3, 100, 250, 100, 50);
        weapons["SteelSword"] = Weapon(0, 30, 5, 1000, 150, 150, 50);
        weapons["MythrilSword"] = Weapon(0, 50, 8, 2000, 200, 50, 150);
        weapons["AdamantiteSword"] = Weapon(0, 75, 10, 5000, 50, 250, 100);
        weapons["DiamondSword"] = Weapon(0, 100, 15, 10000, 750, 500, 1000);

        //armors
        armors["WoodenChest"] = Armor(0, 200, 1, 10, 50, 100, 250);
        armors["IronChest"] = Armor(0, 500, 3, 100, 250, 100, 50);
        armors["SteelChest"] = Armor(0, 800, 5, 1000, 150, 150, 50);
        armors["MythrilChest"] = Armor(0, 1000, 8, 2000, 200, 50, 150);
        armors["AdamantiteChest"] = Armor(0, 1250, 10, 5000, 50, 250, 100);
        armors["DiamondChest"] = Armor(0, 1500, 15, 10000, 750, 500, 1000);

        // Initialize items and prices
        item_prices["potion"] = 500;
        item_prices["wood"] = 4;
        item_prices["stone"] = 8;
        item_prices["iron"] = 6;
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
                cout << "Not enough resources to craft." << item << "." << endl;
            }
        }
        else if (armors.find(item) != armors.end()) {
            if (iron >= armors[item].iron and stone >= armors[item].stone and wood >= armors[item].wood) {
                armors[item].amount += 1;
                cout << item << " is crafted successfully!" << endl;
                Save(username);
            }
            else {
                cout << "Not enough resources to craft." << item << "." << endl;
            }
        }
        else {
            cout << "This item does not exit." << "." << endl;
        }

    }

    void equip() {
        for (const auto& weapon : weapons) {
            if (weapon.second.amount >= 1) {
                cout << weapon.first << ":\t \t \t" << "Level " << weapon.second.level_requirement << " required" << "!\t \t \t" << "Damage" << ":\t \t \t" << weapon.second.base_damage << ":\t \t \t" << "Owned" << ":\t \t \t" << weapon.second.amount << endl;
            }
        }
        for (const auto& armor : armors) {
            if (armor.second.amount >= 1) {
                cout << armor.first << ":\t \t \t" << "Level " << armor.second.level_requirement << " required" << "!\t \t \t" << "Damage" << ":\t \t \t" << armor.second.base_health << ":\t \t \t" << "Owned" << ":\t \t \t" << armor.second.amount << endl;
            }
        }

        string item_name;
        bool isitem = false;
        for (const auto& weapon : weapons) {
            if (weapon.second.amount > 0) {
                isitem = true;
            }
        }
        for (const auto& armor : armors) {
            if (armor.second.amount > 0) {
                isitem = true;
            }
        }
        if (isitem == true) {
            cout << "Enter the item you want to equip: ";
            getline(cin, item_name);

            if (weapons.find(item_name) != weapons.end() and weapons[item_name].amount > 0) {
                if (level * 10 >= weapons[item_name].level_requirement) {
                    equipped_weapon = &weapons[item_name];
                    equipped_sword = item_name;
                    Save(username);
                    cout << "Equipped " << item_name << "." << endl;
                }
                else {
                    cout << "Level too low to equip " << item_name << "." << endl;
                }

            }
            else if (weapons.find(item_name) != weapons.end() and weapons[item_name].amount <= 0) {
                cout << "You don't have " << item_name << "." << endl;
            }
            else if (armors.find(item_name) != armors.end() and armors[item_name].amount > 0) {
                if (level * 10 >= armors[item_name].level_requirement) {
                    equipped_armor = &armors[item_name];
                    equipped_chest = item_name;
                    while (equipped_armor) {
                        health_max = 2000 * level;
                        health_max += armors[equipped_chest].base_health;
                        health = health_max;
                        break;
                    }
                    if (equipped_armor == nullptr) {
                        health_max = 2000 * level;
                        health = health_max;
                    }
                    Save(username);
                    cout << "Equipped " << item_name << "." << endl;
                }
                else {
                    cout << "Level too low to equip " << item_name << "." << endl;
                }

            }
            else if (armors.find(item_name) != armors.end() and armors[item_name].amount <= 0) {
                cout << "You don't have " << item_name << "." << endl;
            }
            else if (armors.find(item_name) == armors.end() and weapons.find(item_name) == weapons.end()) {
                cout << item_name << " does not exist" << endl;
            }

        }
        else {
            cout << "You don't have any weapon." << endl;
        }

    }


    void Reset() {
        state = "normal";
        level = 0.1;
        health = 200;
        health_max = 200;
        potion = 1;
        gold = 1000;
        wood = 1000;
        stone = 500;
        iron = 750;
        equipped_weapon = nullptr;
        for (pair<const basic_string<char>, Weapon>& sword : weapons) {
            sword.second.amount = 0;
        }
        equipped_armor = nullptr;
        for (pair<const basic_string<char>, Armor>& chest : armors) {
            chest.second.amount = 0;
        }
    }
    void quit() {
        login = false;
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
        if (equipped_weapon != nullptr and equipped_armor == nullptr) {
            cout << name << "-san's equipped weapon: " << equipped_sword << endl;
        }
        else if (equipped_armor != nullptr and equipped_weapon == nullptr) {
            cout << name << "-san's equipped armor: " << equipped_chest << endl;
        }
        else if (equipped_armor != nullptr and equipped_weapon != nullptr) {
            cout << name << "-san's equipped weapon: " << equipped_sword << endl;
            cout << name << "-san's equipped armor: " << equipped_chest << endl;
        }
        else {
            cout << "No weapon or armor is equipped " << endl;
        }
    }

    void tired() {
        cout << name << "-san feels tired." << endl;
        health = max(1, health - 10);
        Save(username);
    }
    void drink_potion() {
        if (potion) {
            potion -= 1;
            health += health_max / 3;
            status();
            if (health > health_max) {
                health = health_max;
            }
        }
        else if (potion > 0 and health >= health_max) {
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

        cout << "Enter the item and quantity you want to buy (e.g., 'potion 3' or Wooden Sword 1 or Wooden Chest 1), or type 'cancel' to exit: ";

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

        // Now item will hold the correct item name and quantity the correct number
        cout << "Item: " << item << ", Quantity: " << quantity << endl;

        if (item_prices.find(item) == item_prices.end() and weapons.find(item) == weapons.end() and armors.find(item) == armors.end()) {
            cout << "Item does not exist!" << endl;
            return;
        }
        int total_cost;
        if (item_prices.find(item) != item_prices.end() and weapons.find(item) == weapons.end() and armors.find(item) == armors.end()) {
            total_cost = item_prices[item] * quantity;
            if (gold >= total_cost) {
                // Check which item is being bought and update accordingly
                item_prices[item] += quantity;
                gold -= total_cost;
                cout << "You bought " << quantity << " " << item << "(s) for " << total_cost << " gold." << endl;
                cout << "Remaining gold: " << gold << endl;
                Save(username);
            }
            else {
                cout << "Not enough gold to buy " << quantity << " " << item << "(s)." << endl;
            }
        }
        else if (item_prices.find(item) == item_prices.end() and weapons.find(item) != weapons.end() and armors.find(item) == armors.end()) {
            total_cost = quantity * weapons[item].gold;
            if (gold >= total_cost) {
                // Check which item is being bought and update accordingly
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
        else if (item_prices.find(item) == item_prices.end() and weapons.find(item) == weapons.end() and armors.find(item) != armors.end()) {
            total_cost = quantity * armors[item].gold;
            if (gold >= total_cost) {
                // Check which item is being bought and update accordingly
                armors[item].amount += quantity;
                gold -= total_cost;
                cout << "You bought " << quantity << " " << item << "(s) for " << total_cost << " gold." << endl;
                cout << "Remaining gold: " << gold << endl;
                Save(username);
            }
            else {
                cout << "Not enough gold to buy " << quantity << " " << item << "(s)." << endl;
            }
        }
    }

    void bundle() {
        potion += 10;
        gold += 50000;
        level += 2;
        while (equipped_armor) {
            health_max = 2000 * level;
            health_max += armors[equipped_chest].base_health;
            break;
        }
        if (equipped_armor == nullptr) {
            health_max = 2000 * level;
        }
        health = health_max;
        Save(username);
    }

    void bank() {
        // Initialize items map
        items["potion"] = potion;
        items["gold"] = gold;
        items["wood"] = wood;
        items["stone"] = stone;
        items["iron"] = iron;

        cout << "Items\t \t \tQuantity" << endl;
        for (const auto& item : items) {
            cout << item.first << "\t \t \t" << item.second << endl;
        }

        for (const auto& weapon : weapons) {
            if (weapon.second.amount >= 1) {
                cout << "Owned weapons: " << endl;
                break;
            }
        }

        for (const auto& weapon : weapons) {
            if (weapon.second.amount >= 1) {
                cout << weapon.first << ":\t \t \t" << "Damage" << ":\t \t \t" << weapon.second.base_damage << ":\t \t \t" << "Amount" << ":\t \t \t" << weapon.second.amount << endl;
            }
        }

        for (const auto& armor : armors) {
            if (armor.second.amount >= 1) {
                cout << "Owned armors: " << endl;
                break;
            }
        }

        for (const auto& armor : armors) {
            if (armor.second.amount >= 1) {
                cout << armor.first << ":\t \t \t" << "Health" << ":\t \t \t" << armor.second.base_health << ":\t \t \t" << "Amount" << ":\t \t \t" << armor.second.amount << endl;
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
            if (rand() % 2) {
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
            if (rand() % 2) {
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
            if (rand() % (health + 5) > rand() % (enemy->health + 1)) {
                cout << name << "-san flees from Mr." << enemy->name << "." << endl;
                delete enemy;
                enemy = nullptr;
                state = "normal";
            }
            else {
                cout << name << "-san couldn't escape from Mr." << enemy->name << "!" << endl;
                enemy_attacks();
            }
        }
        Save(username);
    }

    void attack() {
        if (state != "fight") {
            cout << name << "-san swats the air, without notable results." << endl;
            tired();
        }
        else {
            if (do_damage(*enemy)) {
                cout << name << "-san executes Mr." << enemy->name << "!" << endl;
                delete enemy;
                enemy = nullptr;
                state = "normal";
                if (rand() % (health + 1) < health_max) {
                    level += 0.1;
                    while (equipped_armor) {
                        health_max = 2000 * level;
                        health_max += armors[equipped_chest].base_health;
                        break;
                    }
                    if (equipped_armor == nullptr) {
                        health_max = 2000 * level;
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
        if (enemy->do_damage_2(*this)) {
            cout << name << "-san was slaughtered by Mr." << enemy->name << "!!!\nR.I.P." << endl;
            health = 0;
        }
    }
    void Server() {
        // Loop through the userNames map
        for (const auto& userEntry : userNames) {
            int count = 1;
            cout << count << ". ";
            cout << "Username: " << userEntry.first << " , ";
            cout << "Password: " << userEntry.second << endl;
            count += 1;
        }
    }
    void Profile() {
        // Loop through the userNames map
        cout << "Username: " << username << endl;
        cout << "Password: " << password << endl;
    }

    void Login() {
        if (login) {
            cout << "Already logged in as " << username << "!" << endl;
            return;
        }

        string enteredPassword;

        // Prompt for username
        cout << "Enter your username: ";
        cin >> username;

        // Check if the username exists
        while (userNames.find(username) == userNames.end()) {
            cout << "Username not found. Please register first." << endl;
            cout << "Enter your username: ";
            cin >> username;
        }

        // Prompt for password
        cout << "Enter your password: ";
        cin >> enteredPassword;

        // Validate the password
        while (userNames[username] != enteredPassword) {
            cout << "Incorrect password. Please try again." << endl;
            cout << "Enter your password: ";
            cin >> enteredPassword;
        }

        // If the password is correct
        login = true;
        cout << "(type 'get' to get a list of actions)\n";
        Load(username);
        cout << name << "-san enters a dark cave, searching for adventure." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
    }

    void Signup() {
        if (login) {
            cout << "Already logged in as " << username << "!" << endl;
            return;
        }

        // Prompt for character's name
        cout << "What is your character's name? ";
        cin >> name;

        // Prompt for username and ensure it's unique
        while (true) {
            cout << "Enter a username: ";
            cin >> username;

            if (userNames.find(username) != userNames.end()) {
                cout << "Username is unavailable, please try again.\n";
            }
            else {
                break; // Username is available
            }
        }

        // Prompt for password
        cout << "Enter a password: ";
        cin >> password;

        // Store the username and password
        userNames[username] = password;
        login = true;

        cout << "(type 'get' to get a list of actions)\n";
        cout << name << "-san enters a dark cave, searching for adventure." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        Save(username);
        SaveUserData();
    }

    void Save(const string& user) {
        string directory = "UserData/";  // This can be any path like "C:/Games/MyGame/saves/" or relative paths like "saves/"

        // Combine the directory path with the filename (username + "_savegame.txt")
        string filepath = directory + user + "_savegame.txt";

        // Use the username to create a unique save file for that user
        ofstream saveFile(filepath);
        if (!saveFile) {
            cout << "Error opening file for saving!" << endl;
            return;
        }

        // Save player stats
        saveFile << name << endl;
        saveFile << username << endl;
        saveFile << password << endl;
        saveFile << level << endl;
        saveFile << health << endl;
        saveFile << health_max << endl;
        saveFile << potion << endl;
        saveFile << gold << endl;
        saveFile << wood << endl;
        saveFile << stone << endl;
        saveFile << iron << endl;
        if (equipped_armor) {
            saveFile << equipped_armor << endl;
            saveFile << equipped_chest << endl;
        }
        if (equipped_weapon) {
            saveFile << equipped_weapon << endl;
            saveFile << equipped_sword << endl;
        }
        for (const auto& pair : weapons) {
            if (pair.second.base_damage != 0) {
                saveFile << pair.first << " " << pair.second.amount << " " << pair.second.base_damage << " " << pair.second.level_requirement << " " << pair.second.gold << " " << pair.second.iron << " " << pair.second.stone << " " << pair.second.wood << endl;  // username password
            }
        }
        for (const auto& pair : armors) {
            if (pair.second.base_health != 0) {
                saveFile << pair.first << " " << pair.second.amount << " " << pair.second.base_health << " " << pair.second.level_requirement << " " << pair.second.gold << " " << pair.second.iron << " " << pair.second.stone << " " << pair.second.wood << endl;  // username password
            }
        }
        saveFile.close();
        //saveWeaponData();

        //cout << "Game data saved successfully for " << username << "!" << endl;
    }

    void Load(const string& user) {
        string directory = "UserData/";  // This can be any path like "C:/Games/MyGame/saves/" or relative paths like "saves/"
        // Combine the directory path with the filename (username + "_savegame.txt")
        string filepath = directory + user + "_savegame.txt";
        // Use the username to create a unique save file for that user
        ifstream loadFile(filepath);
        if (!loadFile) {
            cout << "No saved game data found for " << user << ". Starting new game." << endl;
            return;
        }

        // Load player stats
        loadFile >> name;
        loadFile >> username;
        loadFile >> password;
        loadFile >> level;
        loadFile >> health;
        loadFile >> health_max;
        loadFile >> potion;
        loadFile >> gold;
        loadFile >> wood;
        loadFile >> stone;
        loadFile >> iron;
        loadFile >> equipped_chest;
        loadFile >> equipped_sword;
        equipped_armor = &armors[equipped_chest];
        equipped_weapon = &weapons[equipped_sword];


        string name;
        int base, lvl, gold, iron, stone, wood, count;
        while (loadFile >> name >> count >> base >> lvl >> gold >> iron >> stone >> wood) {
            if (weapons.find(name) != weapons.end()) {
                weapons[name] = Weapon(count, base, lvl, gold, iron, stone, wood);
            }
            if (armors.find(name) != armors.end()) {
                armors[name] = Armor(count, base, lvl, gold, iron, stone, wood);
            }
            //weapons[name] = Weapon(count, hit, lvl, gold, iron, stone, wood);
        }

        loadFile.close();
        //loadWeaponData();

        //cout << "Game data loaded successfully for " << username << "!" << endl;
    }

    void SaveUserData() {
        ofstream userFile("userdata.txt");
        if (!userFile) {
            cout << "Error opening file for saving usernames!" << endl;
            return;
        }

        // Save the username-password pairs
        for (const auto& pair : userNames) {
            userFile << pair.first << " " << pair.second << endl;  // username password
        }

        userFile.close();
        //cout << "User data saved successfully!" << endl;
    }

    void LoadUserData() {
        ifstream userFile("userdata.txt");
        if (!userFile) {
            //cout << "No user data found. Starting fresh." << endl;
            return;
        }

        userNames.clear();  // Clear the map before loading new data
        string loadedUsername, loadedPassword;
        while (userFile >> loadedUsername >> loadedPassword) {
            userNames[loadedUsername] = loadedPassword;
        }

        userFile.close();
    }
    /*void saveWeaponData(){
        ofstream weaponFile("weaponFile.txt");
        if (!weaponFile) {
            cout << "Error opening file for saving weapons!" << endl;
            return;
        }

        // Save the username-password pairs
        for (const auto& pair : weapons) {
            if (pair.second.base_damage != 0){
                weaponFile << pair.first << " " << pair.second.amount << " " << pair.second.base_damage<< " " << pair.second.level_requirement<< " " << pair.second.gold<< " " << pair.second.iron<< " " << pair.second.stone<< " " << pair.second.wood<< endl;  // username password
            }
        }
        weaponFile.close();
        saveArmorData();
    }

    void loadWeaponData(){
        ifstream weaponFile("weaponFile.txt");
        if (!weaponFile) {
            //cout << "No user data found. Starting fresh." << endl;
            return;
        }
        //weapons.clear();  // Clear the map before loading new data
        string name;
        int hit, lvl, gold, iron, stone, wood, count;
        while (weaponFile >> name >> count >> hit>> lvl>>gold>>iron>>stone>>wood) {
            weapons[name] = Weapon(count, hit, lvl, gold, iron, stone, wood);
        }
        weaponFile.ignore(); // Ignore the newline character left in the buffer
        weaponFile.close();
        loadArmorData();
    }

    void saveArmorData(){
        ofstream armorFile("armorFile.txt");
        if (!armorFile) {
            cout << "Error opening file for saving weapons!" << endl;
            return;
        }
        for (const auto& pair : armors) {
            if (pair.second.base_health != 0){
                armorFile << pair.first  << " " << pair.second.amount << " " << pair.second.base_health<< " " << pair.second.level_requirement<< " " << pair.second.gold<< " " << pair.second.iron<< " " << pair.second.stone<< " " << pair.second.wood<< endl;  // username password
            }
        }
        armorFile.close();
    }

    void loadArmorData(){
        ifstream armorFile("armorFile.txt");
        if (!armorFile) {
            //cout << "No user data found. Starting fresh." << endl;
            return;
        }
        //armors.clear();  // Clear the map before loading new data
        string first;
        int hp, lvl, gold, iron, stone, wood, count;
        while (armorFile >> first >> count >> hp>> lvl>>gold>>iron>>stone>>wood) {
            armors[first] = Armor(count, hp, lvl, gold, iron, stone, wood);
        }
        armorFile.ignore(); // Ignore the newline character left in the buffer
        armorFile.close();
    }*/

};
int main() {

    srand(static_cast<unsigned int>(time(0)));

    Player p;
    p.LoadUserData();
    while (p.equipped_armor) {
        p.health_max += p.armors[p.equipped_chest].base_health;
        break;
    }
    while (p.equipped_armor == nullptr) {
        p.health_max = 2000 * p.level;
        break;
    }
    p.Reset();
    cout << "Type 'register' if you are new or Type 'login' to enter: ";

    string line;
    while (p.health > 0) {
        cout << "> ";
        getline(cin, line);
        bool commandFound = false;

        for (const auto& command : p.commands) {
            if (line.find(command.first) == 0 and p.login != true and line != "register" and line != "login") {
                cout << "Type 'register' if you are new or Type 'login' to enter: ";
                commandFound = true;
            }
            else if (line.find(command.first) == 0 and (p.login == true or (line == "register" or line == "login"))) {
                (p.*(command.second))();
                commandFound = true;
                break;
            }
        }
        if (!commandFound and p.login == true) {
            cout << p.name << "-san doesn't understand the suggestion." << endl;
        }
        else if (!commandFound and p.login != true) {
            cout << "Invalid Command!" << endl;
            cout << "Type 'register' if you are new or Type 'login' to enter: ";
        }
    }
    return 0;
}