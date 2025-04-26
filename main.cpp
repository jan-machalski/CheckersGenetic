#include <iostream>
#include "Board.h"
#include "Utils.hpp"
#include "Game.hpp"

using namespace std;

int main()
{
	MinimaxPlayer testPlayer(true, 500,4);
	uint32_t move = testPlayer.MakeMove();

	cout << "Best move: " << move << endl;
	/*MoveGenerator::Init();

	MonteCarloPlayer whitePlayer(true, 300);
	MonteCarloPlayer blackPlayer(false, 300);
	Game game(&whitePlayer, &blackPlayer);
	auto future = game.SimulateAsync();

	auto [result, moves] = future.get();

	switch (result)
	{
		case Game::WHITE_WINS:
			cout << "White wins!" << endl;
			break;
		case Game::BLACK_WINS:
			cout << "Black wins!" << endl;
			break;
		case Game::DRAW:
			cout << "Draw!" << endl;
			break;
	}

	for (const auto& move : moves)
	{
		cout << move << endl;
	}*/
	return 0;
}


