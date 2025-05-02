#include "HumanPlayer.hpp"

#include <iostream>

using namespace std;

uint32_t HumanPlayer::MakeMove()
{
	string moveInput;
	Move* madeMove = nullptr;
	const string displayMovesCommand = "moves";
	auto availableMoves = board.GenerateMovesWithNotation();
	if (availableMoves.empty()) return 0;
	do
	{
		printf("Input your move (e.g. c3-d4 or e3:g5:e7 or input \"%s\" to display available moves\n",displayMovesCommand);
		cin >> moveInput;
		if (moveInput == displayMovesCommand)
		{
			printf("Available moves:\n");
			for (auto move : availableMoves)
			{
				printf("%s\n", move.notation);
			}
			printf("\n");
			continue;
		}
		madeMove = find_if(availableMoves.begin(), availableMoves.end(), [moveInput](const Move& move) {return moveInput == move.notation; });

	} while (madeMove == availableMoves.end());
	board.ApplyMove(madeMove->mask);

	return madeMove->mask;
}

void HumanPlayer::InputMove(uint32_t moveMask)
{
	board.ApplyMove(moveMask);
}