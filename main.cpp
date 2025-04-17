// CheckersGenetic.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include "Board.h"
#include "Utils.hpp"
#include "MonteCarloPlayer.hpp"

using namespace std;

int main()
{
	MoveGenerator::Init();

	MonteCarloPlayer whitePlayer(true, 1000);
	MonteCarloPlayer blackPlayer(false, 2000);
	Board gameBoard;
	uint32_t newMove = 0;
	int moveCounter = 1;

	while (true)
	{
		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
			cout << "Draw!" << endl;
			break;
		}

		printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);

		cout << "Move number: " << moveCounter++ << endl;

		auto availableMoves = gameBoard.GenerateMovesWithNotation();
		
		newMove = whitePlayer.MakeMove();
		if (newMove == 0)
		{
			cout << "White player has no moves left!" << endl;
			break;
		}

		auto moveMade = find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			cout << "Invalid move made by white player!" << endl;
			break;
		}
		gameBoard.ApplyMove(newMove);
		blackPlayer.InputMove(newMove);

		cout << "White player made move: " << moveMade->notation << endl;
		printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);
		
		availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = blackPlayer.MakeMove();
		if (newMove == 0)
		{
			cout << "Black player has no moves left!" << endl;
			break;
		}
		moveMade = find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			cout << "Invalid move made by black player!" << endl;
			break;
		}

		gameBoard.ApplyMove(newMove);
		whitePlayer.InputMove(newMove);

		cout << "Black player made move: " << moveMade->notation << endl;

	}

	return 0;
}


