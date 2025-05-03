#include <iostream>
#include <vector>
#include "Game.hpp"
#include "Evolution/EvolutionManager.hpp"

using namespace std;

int main() {
    MoveGenerator::Init();

    char choice;
    bool resume = false;

    printf("Resume from last saved evolution state? (y/n): ");
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        resume = true;
    }

    EvolutionManager evolution(resume);
    evolution.Run();

    return 0;
}
