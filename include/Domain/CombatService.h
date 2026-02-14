#pragma once
#include "Entities/Entity.h"
#include "Core/RNG.h"
#include "Core/GVM.h"
#include "Data/ItemDatabase.h"
#include "Data/EnemyDatabase.h"
#include "Domain/AI/QLearningAgent.h"

namespace Domain {

class CombatService {
public:
    CombatService();
    void Initialize(); // Training

    // Returns true if player wins, false if died or fled
    // In a real ECS engine, this would process one tick.
    // Here, we simulate the whole fight or a turn.
    // For this refactor, let's keep it turn-based blocking for simplicity, 
    // or return a state.
    // Let's stick to the Game class handling the loop, and this service handling one turn.
    
    struct CombatResult {
        bool player_died = false;
        bool enemy_died = false;
        // bool escaped = false; // Removed 'escaped' member
        std::vector<std::string> log;
    };

    CombatResult ExecuteTurn(Entities::GameEntity& player, Entities::GameEntity& enemy, const std::string& action); // Changed parameter name to 'action'

private:
    int CalculateDamage(const Entities::GameEntity& attacker, const Entities::GameEntity& defender);
    std::vector<uint8_t> CompileDamageScript(); // Added new private method
    AI::QLearningAgent m_aiAgent;
    Core::GVM m_vm; // Added GVM member
};

} // namespace Domain
