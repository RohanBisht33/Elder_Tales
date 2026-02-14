#pragma once
#include <memory>
#include <string>
#include <deque>

#include "Entities/Entity.h"
#include "Domain/CombatService.h"
#include "Core/RNG.h"

namespace Core {

enum class GameState {
    Login,
    MainMenu,
    Explore,
    Combat,
    Inventory,
    GameOver,
    Shop,
    Exit
};

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    void HandleLogin();
    void HandleMainMenu();
    void HandleExplore();
    void HandleCombat();
    void HandleInventory();
    void HandleRest();
    void HandleProfile();
    void HandleHelp();
    void HandleShop();
    void HandleCraft();
    void HandleBank();
    void HandleAdmin();
    void HandlePotion();
    void HandleMining();
    void HandleFishing();
    void HandleTournament();
    void HandleInput();

    bool m_running;
    GameState m_state;
    std::shared_ptr<RNG> m_rng;
    
    // Player Entity (for Phase 1, just one)
    Entities::GameEntity m_player;
    Domain::CombatService m_combatService;
    Entities::GameEntity m_current_enemy;
    
    // UI Helpers
    std::deque<std::string> m_message_log;
    void AddLog(const std::string& message);
};

} // namespace Core
