#include <iostream>
#include <vector>
#include "Game.hpp"
#include "Evolution/EvolutionManager.hpp"

using namespace std;
int main() {

    MoveGenerator::Init();
    EvolutionManager evolution;
    evolution.Run();
    return 0;
}
