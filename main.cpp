#include <iostream>
#include <vector>
#include "Bot.hpp"
#include "Evolution.hpp"
#include "EvolutionConfig.hpp"
#include "Game.hpp"
#include "EvolutionManager.hpp"

using namespace std;
int main() {
    MoveGenerator::Init();
    EvolutionManager evolution;
    evolution.Run();
    return 0;
}
