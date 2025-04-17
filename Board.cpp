#include "Board.h"
#include "Utils.hpp"

CheckersVector<uint32_t> Board::GenerateMoves() const
{
	return MoveGenerator::GenerateMoves(whitePieces, blackPieces, promotedPieces, isWhiteTurn);
}

CheckersVector<Move> Board::GenerateMovesWithNotation() const
{
	return MoveGenerator::GenerateMovesWithNotation(whitePieces, blackPieces, promotedPieces, isWhiteTurn);
}

void Board::Print()
{
	printBoard(whitePieces, blackPieces, promotedPieces, true);
}

Board Board::MakeMove(uint32_t moveMask) //return copy of the board after making move
{
	Board newBoard = *this;
	newBoard.ApplyMove(moveMask);
	return newBoard;
}

void Board::ApplyMove(uint32_t moveMask)
{
	uint32_t ourPieces = isWhiteTurn ? whitePieces : blackPieces;
	uint32_t opponentPieces = isWhiteTurn ? blackPieces : whitePieces;
	uint32_t startingSquare = ourPieces & moveMask;
	uint32_t capturedPieces = opponentPieces & moveMask;
	uint32_t targetSquare = moveMask & ~(ourPieces | opponentPieces);
	if (!targetSquare) targetSquare = moveMask & ourPieces;
	bool queenUsed = startingSquare & promotedPieces;

	opponentPieces &= ~capturedPieces;
	promotedPieces &= ~capturedPieces;
	ourPieces ^= startingSquare;
	ourPieces |= targetSquare;

	uint32_t promotionMask = isWhiteTurn ? WHITE_PROMOTION_MASK : BLACK_PROMOTION_MASK;
	promotedPieces |= targetSquare & promotionMask;
	if (promotedPieces & startingSquare)
		promotedPieces = (promotedPieces ^ startingSquare) | targetSquare;
	if (isWhiteTurn)
	{
		whitePieces = ourPieces;
		blackPieces = opponentPieces;
	}
	else
	{
		blackPieces = ourPieces;
		whitePieces = opponentPieces;
	}

	isWhiteTurn = !isWhiteTurn;
	nonAdvancingMoveCount = (capturedPieces || !queenUsed) ? 0 : (nonAdvancingMoveCount + 1);
}