#include "Core/Game.h"
#include <iostream>

int main() {
    try {
        Core::Game game;
        game.Run();
    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
