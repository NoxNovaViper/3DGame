#include "game/Game.h"
#include <iostream>

int main() {
    std::cout << "Starting SAO: Aincrad..." << std::endl;
    
    Game game;
    game.Initialize();
    game.Run();
    game.Shutdown();
    
    std::cout << "SAO: Aincrad closed." << std::endl;
    return 0;
}
