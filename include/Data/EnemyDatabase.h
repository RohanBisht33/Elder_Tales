#pragma once
#include <map>
#include <string>

namespace Data {

struct EnemyData {
    std::string name;
    int base_hp;
    int base_damage;
    float level_mult;
    int xp_reward;
};

class EnemyDatabase {
public:
    static void Initialize();
    static const EnemyData* GetEnemy(const std::string& name);
    static const EnemyData* GetRandomEnemy(int player_level); // Placeholder logic

private:
    static std::map<std::string, EnemyData> m_enemies;
};

} // namespace Data
