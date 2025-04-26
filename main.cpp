#include <iostream>
#include "Board.h"
#include "Utils.hpp"
#include "Game.hpp"

using namespace std;

int main()
{
	MoveGenerator::Init();
	MinimaxPlayer whitePlayer(true, 200,30);
	EvaluationWeights weights(1.0, 1.0);
	MinimaxPlayer blackPlayer(false, 200,weights, 30);

	Game game(&whitePlayer, &blackPlayer);
	game.Play();
	/*auto future = game.SimulateAsync();

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


