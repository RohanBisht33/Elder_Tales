#pragma once

#include <map>
#include <vector>
#include <tuple>
#include <iostream>

namespace Domain::AI {

struct AIState {
    int playerHpPercent; // 0-10
    int aiHpPercent;     // 0-10

    bool operator<(const AIState& other) const {
        return std::tie(playerHpPercent, aiHpPercent) < std::tie(other.playerHpPercent, other.aiHpPercent);
    }
};

enum class AIAction {
    ATTACK,
    HEAL
};

class QLearningAgent {
public:
    QLearningAgent();
    void Train(int episodes = 10000);
    AIAction ChooseAction(const AIState& state);
    void UpdateQTable(const AIState& state, AIAction action, const AIState& nextState, int reward);

private:
    std::map<std::pair<AIState, AIAction>, double> m_qTable;
    double m_alpha = 0.1;  // Learning rate
    double m_gamma = 0.9;  // Discount factor
    double m_epsilon = 0.1; // Exploration rate
};

}
