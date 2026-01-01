#include "ai_opponent.h"
#include <iostream>

using namespace std;

int main() {
    AIOpponent ai;

    // Train the AI
    ai.train();

    // Play a single game
    State state = {100, 100};  // Player and AI health
    cout << "Game start! Player: " << state.playerHealth << ", AI: " << state.aiHealth << endl;

    while (state.playerHealth > 0 && state.aiHealth > 0) {
        // AI's turn
        Action aiAction = ai.chooseAction(state);
        if (aiAction == Action::ATTACK) {
            state.playerHealth -= 10;
            cout << "AI attacks! Player health: " << state.playerHealth << endl;
        } else if (aiAction == Action::HEAL) {
            state.aiHealth = min(state.aiHealth + 10, 100);
            cout << "AI heals! AI health: " << state.aiHealth << endl;
        }

        // Player's turn (simple attack for now)
        if (state.playerHealth > 0) {
            state.aiHealth -= 10;
            cout << "Player attacks! AI health: " << state.aiHealth << endl;
        }
    }

    if (state.playerHealth <= 0) cout << "AI wins!" << endl;
    else cout << "Player wins!" << endl;

    return 0;
}
