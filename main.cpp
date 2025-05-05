#include <iostream>
#include <vector>
#include "Game.hpp"
#include "EvolutionManager.hpp"

using namespace std;

void RunEvolution()
{
    char choice;
    bool resume = false;

    printf("Resume from last saved evolution state? (y/n): ");
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        resume = true;
    }

    EvolutionManager evolution(resume);
    evolution.Run();
}

std::unique_ptr<Player> CreatePlayer(bool white) {

	printf("Choose player for %s\n", white ? "white" : "black");
	printf("1. Minimax\n");
	printf("2. Monte Carlo\n");
	printf("3. Human\n");

	char choice;
	uint16_t timePerMove = 0;
	cin >> choice;
	switch (choice) {
	case '1':
		printf("Choose time per move in milliseconds: ");
		cin >> timePerMove;
		return std::make_unique<MinimaxPlayer>(white, timePerMove);
	case '2':
		printf("Choose time per move in milliseconds: ");
		cin >> timePerMove;
		return std::make_unique<MonteCarloPlayer>(white, timePerMove);
	case '3':
		return std::make_unique<HumanPlayer>(white);
	}
}

void PlaySingleGame()
{
	auto whitePlayer = CreatePlayer(true);
	auto blackPlayer = CreatePlayer(false);
	Game game(whitePlayer.get(), blackPlayer.get());
	game.Play();
}

int main() {
    MoveGenerator::Init();

    char choice;
    do
    {
		printf("1. Run Evolution\n");
        printf("2. Play single game\n");
		printf("3. Exit\n");    
		cin >> choice;
		switch (choice)
		{
		case '1':
			RunEvolution();
			break;
		case '2':
			PlaySingleGame();	
			break;
		case '3':
			printf("Exiting...\n");
			break;
		default:
			printf("Invalid choice. Please try again.\n");
		}
    } while (choice != '3');

    return 0;
}
