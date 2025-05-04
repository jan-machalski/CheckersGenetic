#pragma once
#include "Board.h"

//#define PRINT_EVALUATIONS

class Player 
{
public:
	const bool isWhite;

	Player(bool isWhite) : isWhite(isWhite), board(true) {}

	virtual ~Player() {}

	virtual uint32_t MakeMove() = 0; // choose move, make it and return its mask
	virtual void InputMove(uint32_t moveMask) = 0; // input opponents move
protected:
	Board board;
};

