#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace std;
class Character {
public:
    string name;
    int health;
    int health_max;
    int damage;

    Character() : name(""), health(200), health_max(200), damage(0) {}

    bool do_damage(Character& enemy) {
        damage = min(max(rand() % (health + 1) - rand() % (enemy.health + 1), 0), enemy.health);
        enemy.health -= damage;
        if (damage == 0)
            cout << "Mr." << enemy.name << " evades " << name << "-san's attack." << endl;
        else {
            cout << name << "-san hurts Mr." << enemy.name << "!" << endl;
            cout << "Damage by player: " << damage << endl;
        }
        return enemy.health <= 0;
    }

    bool do_damage_2(Character& self) {
        cout << "Mr." << name << "'s health: " << health << "/" << health_max << endl;
        int damage_2 = min(max(rand() % (health + 1) - rand() % (self.health + 1), 0), self.health);
        self.health -= damage_2;
        if (damage_2 == 0)
            cout << self.name << "-san evades Mr." << name << " attack." << endl;
        else
            cout << name << "-san hurts Mr." << self.name << "!" << endl;
        return self.health <= 0;
    }
};

class Enemy : public Character {
public:
    Enemy(Character& player) {
        if (rand() % 2) {
            name = "High orcs";
            health_max = rand() % (player.health + 1) + 100;
        } else if (rand() % 2) {
            name = "Majin";
            health_max = rand() % (player.health + 1) + 150;
        } else if (rand() % 2) {
            name = "Kajin";
            health_max = rand() % (player.health + 1) + 170;
        } else {
            name = "Demon Lord";
            health_max = rand() % (player.health + 1) + 200;
        }
        health = health_max;
    }
};

class Player : public Character {
public:
    string state;
    Enemy* enemy = nullptr;

    Player() {
        state = "normal";
        health = 200;
        health_max = 200;
    }

    void quit() {
        cout << name << "-san can't find the way back home, and dies of starvation.\nR.I.P." << endl;
        health = 0;
    }

    void help() {
        cout << "Available commands: quit, help, status, rest, explore, flee, attack" << endl;
    }

    void status() {
        cout << name << "-san's health: " << health << "/" << health_max << endl;
    }

    void tired() {
        cout << name << "-san feels tired." << endl;
        health = max(1, health - 10);
    }

    void rest() {
        if (state != "normal") {
            cout << name << "-san can't rest now!" << endl;
            enemy_attacks();
        } else {
            if (rand() % 2) {
                enemy = new Enemy(*this);
                cout << name << "-san is rudely awakened by Mr." << enemy->name << "!" << endl;
                state = "fight";
                enemy_attacks();
            } else {
                cout << name << "-san rests." << endl;
                if (health < health_max)
                    health = health_max;
                else {
                    cout << name << "-san slept too much." << endl;
                    health -= 10;
                }
            }
        }
    }

    void explore() {
        if (state != "normal") {
            cout << name << "-san is too busy right now!" << endl;
            enemy_attacks();
        } else {
            if (rand() % 2) {
                enemy = new Enemy(*this);
                cout << name << "-san encounters a " << enemy->name << "!" << endl;
                state = "fight";
            } else {
                cout << name << "-san explores a twisty passage." << endl;
            }
        }
    }

    void flee() {
        if (state != "fight") {
            cout << name << "-san runs in circles for a while." << endl;
            tired();
        } else {
            if (rand() % (health + 5) > rand() % (enemy->health + 1)) {
                cout << name << "-san flees from Mr." << enemy->name << "." << endl;
                delete enemy;
                enemy = nullptr;
                state = "normal";
            } else {
                cout << name << "-san couldn't escape from Mr." << enemy->name << "!" << endl;
                enemy_attacks();
            }
        }
    }

    void attack() {
        if (state != "fight") {
            cout << name << "-san swats the air, without notable results." << endl;
            tired();
        } else {
            if (do_damage(*enemy)) {
                cout << name << "-san executes Mr." << enemy->name << "!" << endl;
                delete enemy;
                enemy = nullptr;
                state = "normal";
                if (rand() % (health + 1) < health_max) {
                    health += 50;
                    cout << "Damage before leveling: " << damage << endl;
                    damage += rand() % 11 + 20;
                    health_max += 50;
                    cout << name << "-san feels stronger!" << endl;
                }
            } else {
                enemy_attacks();
            }
        }
    }

    void enemy_attacks() {
        if (enemy->do_damage_2(*this)) {
            cout << name << "-san was slaughtered by Mr." << enemy->name << "!!!\nR.I.P." << endl;
            health = 0;
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0)));

    Player p;
    cout << "What is your character's name? ";
    std::getline(cin, p.name);
    cout << "(type help to get a list of actions)\n";
    cout << p.name << "-san enters a dark cave, searching for adventure." << endl;

    map<string, void(Player::*)()> commands = {
            {"quit", &Player::quit},
            {"help", &Player::help},
            {"status", &Player::status},
            {"rest", &Player::rest},
            {"explore", &Player::explore},
            {"flee", &Player::flee},
            {"attack", &Player::attack}
    };

    string line;
    while (p.health > 0) {
        cout << "> ";
        getline(cin, line);
        bool commandFound = false;
        for (auto& command : commands) {
            if (line.find(command.first) == 0) {
                (p.*(command.second))();
                commandFound = true;
                break;
            }
        }
        if (!commandFound) {
            cout << p.name << "-san doesn't understand the suggestion." << endl;
        }
    }

    return 0;
}
