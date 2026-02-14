#include "Domain/AI/QLearningAgent.h"
#include "Core/ServiceLocator.h"

namespace Domain::AI {

QLearningAgent::QLearningAgent() {}

AIAction QLearningAgent::ChooseAction(const AIState& state) {
    // Epsilon-greedy
    if (static_cast<double>(Core::ServiceLocator::GetRNG().float01()) < m_epsilon) {
        return static_cast<AIAction>(Core::ServiceLocator::GetRNG().range(0, 1));
    } else {
        double qAttack = m_qTable[{state, AIAction::ATTACK}];
        double qHeal = m_qTable[{state, AIAction::HEAL}];

        if (m_qTable.find({state, AIAction::ATTACK}) == m_qTable.end() && 
            m_qTable.find({state, AIAction::HEAL}) == m_qTable.end()) {
             return AIAction::ATTACK; // Default
        }

        if (qAttack >= qHeal) return AIAction::ATTACK;
        else return AIAction::HEAL;
    }
}

void QLearningAgent::UpdateQTable(const AIState& state, AIAction action, const AIState& nextState, int reward) {
    double currentQ = m_qTable[{state, action}];
    double maxFutureQ = std::max(m_qTable[{nextState, AIAction::ATTACK}], m_qTable[{nextState, AIAction::HEAL}]);
    m_qTable[{state, action}] = currentQ + m_alpha * (reward + m_gamma * maxFutureQ - currentQ);
}

void QLearningAgent::Train(int episodes) {
    std::cout << "Training AI Opponent..." << std::endl;
    for (int i = 0; i < episodes; ++i) {
        int hpPlayer = 100;
        int hpAI = 100;

        while (hpPlayer > 0 && hpAI > 0) {
            AIState state = { hpPlayer / 10, hpAI / 10 };
            AIAction action = ChooseAction(state);

            int reward = 0;
            if (action == AIAction::ATTACK) {
                hpPlayer -= 10; 
                reward = 10;
            } else {
                hpAI = std::min(100, hpAI + 20);
                reward = -5;
            }

            // Opponent turn (simple logic)
            hpAI -= 8;

            // Terminal rewards
            if (hpPlayer <= 0) reward += 50;
            if (hpAI <= 0) reward -= 50;

            AIState nextState = { std::max(0, hpPlayer) / 10, std::max(0, hpAI) / 10 };
            UpdateQTable(state, action, nextState, reward);

            hpPlayer = std::max(0, hpPlayer);
            hpAI = std::max(0, hpAI);
        }
    }
    std::cout << "AI Training Completed." << std::endl;
}

}
