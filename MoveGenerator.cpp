#pragma once
#include "MoveGenerator.h"
#include "Utils.hpp"

using namespace std;

void MoveGenerator::Init() 
{
	for (int i = 0; i < 32; i++)
	{
		whitePawnMoveMasks[i] = GenerateWhitePawnMoveMask(i);
		blackPawnMoveMasks[i] = GenerateBlackPawnMoveMask(i);
		rightUpSquareMasks[i] = GenerateRightUpSquareMask(i);
		leftUpSquareMasks[i] = GenerateLeftUpSquareMask(i);
		rightDownSquareMasks[i] = GenerateRightDownSquareMask(i);
		leftDownSquareMasks[i] = GenerateLeftDownSquareMask(i);
	}
	
}

uint32_t MoveGenerator::GenerateWhitePawnMoveMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (position & PLUS_3_MOVE_AVAILABLE) mask |= (position << 3);
	if (position & PLUS_5_MOVE_AVAILABLE) mask |= (position << 5);
	if (position & PLUS_4_MOVE_AVAILABLE) mask |= (position << 4);

	return mask;
}
uint32_t MoveGenerator::GenerateBlackPawnMoveMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (position & MINUS_3_MOVE_AVAILABLE) mask |= (position >> 3);
	if (position & MINUS_5_MOVE_AVAILABLE) mask |= (position >> 5);
	if (position & MINUS_4_MOVE_AVAILABLE) mask |= (position >> 4);
	return mask;
}
uint32_t MoveGenerator::GenerateRightUpSquareMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (!(position & RIGHT_UP_MOVE_AVAILABLE)) return mask;

	if (PLUS_5_MOVE_AVAILABLE & position) mask |= (position << 5);
	else mask |= (position << 4);

	return mask;
}
uint32_t MoveGenerator::GenerateLeftUpSquareMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (!(position & LEFT_UP_MOVE_AVAILABLE)) return mask;

	if (PLUS_3_MOVE_AVAILABLE & position) mask |= (position << 3);
	else mask |= (position << 4);

	return mask;
}
uint32_t MoveGenerator::GenerateRightDownSquareMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (!(position & RIGHT_DOWN_MOVE_AVAILABLE)) return mask;

	if (MINUS_3_MOVE_AVAILABLE & position) mask |= (position >> 3);
	else mask |= (position >> 4);
	return mask;
}
uint32_t MoveGenerator::GenerateLeftDownSquareMask(int squareIndex)
{
	uint32_t mask = 0;
	uint32_t position = 1u << squareIndex;
	if (!(position & LEFT_DOWN_MOVE_AVAILABLE)) return mask;

	if (MINUS_5_MOVE_AVAILABLE & position) mask |= (position >> 5);
	else mask |= (position >> 4);
	return mask;
}
void MoveGenerator::GenerateQueenCapture(const Direction& dir, uint32_t currentPos, uint32_t blockers, uint32_t capturableEnemyPieces, uint32_t currentCapture, uint32_t ourPieces, uint32_t enemyPieces, CheckersQueue<uint32_t>& captureQueue, bool& furtherCaptures)
{
	uint32_t index = (currentPos & dir.moveAvailableMask) ? currentPos : 0;
	uint32_t capturedPiece = 0;
	while (index & dir.moveAvailableMask)
	{
		index = index & dir.moveTypeMask ? Shift(index, dir.shiftA) : Shift(index, dir.shiftB);
		;
		if (index & blockers)
			break;
		else if (index & capturableEnemyPieces)
		{
			if (capturedPiece) break;
			capturedPiece = index;
		}
		else if (capturedPiece)
		{
			captureQueue.push((currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | index);
			furtherCaptures = true;
		}
	}
}

 CheckersVector<uint32_t> MoveGenerator::GenerateMoves(uint32_t whitePieces, uint32_t blackPieces, uint32_t promotedPieces, bool isWhiteTurn)
{
	uint32_t ourPieces = isWhiteTurn ? whitePieces : blackPieces;
	uint32_t enemyPieces = isWhiteTurn ? blackPieces : whitePieces;
	uint32_t emptySquares = ~(ourPieces | enemyPieces);

	CheckersVector<uint32_t> nonCaptureMoves;
	CheckersQueue<uint32_t> captureMoves;

	uint32_t ourQueens = ourPieces & promotedPieces;
	uint32_t ourPawns = ourPieces & ~ourQueens;

	while (ourQueens)
	{
		int square = countr_zero(ourQueens);
		uint32_t squareMask = 1u << square;


		for (const auto& dir : QueenDirs)
		{
			uint32_t index = squareMask;
			uint32_t enemyPiecePassed = 0;

			while (index & dir.moveAvailableMask)
			{
				index = (index & dir.moveTypeMask) ? Shift(index, dir.shiftA) : Shift(index, dir.shiftB);

				if (index & ourPieces)
					break;
				else if (index & enemyPieces)
				{
					if (enemyPiecePassed) break;
					enemyPiecePassed = index;
				}
				else if (enemyPiecePassed)
				{
					captureMoves.push(squareMask | enemyPiecePassed | index);
				}
				else
					nonCaptureMoves.push_back(squareMask | index);
			}
		}

		ourQueens = ourQueens & (ourQueens - 1);
	}

	while (ourPawns)
	{
		int square = countr_zero(ourPawns);
		uint32_t squareMask = 1u << square;

		uint32_t moves = emptySquares & (isWhiteTurn ? whitePawnMoveMasks[square] : blackPawnMoveMasks[square]);
		while (moves)
		{
			nonCaptureMoves.push_back(squareMask | (1u << countr_zero(moves)));
			moves = moves & (moves - 1);
		}

		// right up capture
		if (squareMask & RIGHT_UP_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & PLUS_5_MOVE_AVAILABLE ? squareMask << 5 : squareMask << 4;
			uint32_t newSquare = capturedPieceSquare & PLUS_5_MOVE_AVAILABLE ? capturedPieceSquare << 5 : capturedPieceSquare << 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push(squareMask | capturedPieceSquare | newSquare);
		}
		
		// left up capture
		if (squareMask & LEFT_UP_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & PLUS_3_MOVE_AVAILABLE ? squareMask << 3 : squareMask << 4;
			uint32_t newSquare = capturedPieceSquare & PLUS_3_MOVE_AVAILABLE ? capturedPieceSquare << 3 : capturedPieceSquare << 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push(squareMask | capturedPieceSquare | newSquare);
		}

		// right down capture
		if (squareMask & RIGHT_DOWN_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & MINUS_3_MOVE_AVAILABLE ? squareMask >> 3 : squareMask >> 4;
			uint32_t newSquare = capturedPieceSquare & MINUS_3_MOVE_AVAILABLE ? capturedPieceSquare >> 3 : capturedPieceSquare >> 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push(squareMask | capturedPieceSquare | newSquare);
		}

		// left down capture
		if (squareMask & LEFT_DOWN_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & MINUS_5_MOVE_AVAILABLE ? squareMask >> 5 : squareMask >> 4;
			uint32_t newSquare = capturedPieceSquare & MINUS_5_MOVE_AVAILABLE ? capturedPieceSquare >> 5 : capturedPieceSquare >> 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push(squareMask | capturedPieceSquare | newSquare);
		}

		ourPawns = ourPawns & (ourPawns - 1);
	}
	if (captureMoves.empty())
		return nonCaptureMoves;
	else
		return GenerateWholeCaptures(ourPieces, enemyPieces, promotedPieces, captureMoves);
}

CheckersVector<uint32_t> MoveGenerator::GenerateWholeCaptures(uint32_t ourPieces, uint32_t enemyPieces, uint32_t promotedPieces, CheckersQueue<uint32_t> captureQueue)
{
	CheckersVector<uint32_t> fullCaptures;

	while (!captureQueue.empty())
	{
		bool furtherCaptures = false;
		uint32_t currentCapture = captureQueue.front();
		captureQueue.pop();

		uint32_t capturableEnemyPieces = enemyPieces & ~(currentCapture);
		uint32_t currentPos = (currentCapture & (~(ourPieces | enemyPieces)));
		if (!currentPos) currentPos = currentCapture & ourPieces;
		uint32_t blockers = (enemyPieces & currentCapture) | (ourPieces & ~currentCapture);
		uint32_t freeSquares = (~(ourPieces | enemyPieces) | (ourPieces & currentCapture)) & (~currentPos);

		// queen captures
		if (promotedPieces & ourPieces & currentCapture)
		{
			for (const auto& dir : QueenDirs)
				GenerateQueenCapture(dir, currentPos, blockers, capturableEnemyPieces, currentCapture, ourPieces, enemyPieces, captureQueue, furtherCaptures);
		}
		// pawn captures
		else
		{
			// right up capture
			if (currentPos & RIGHT_UP_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & PLUS_5_MOVE_AVAILABLE ? currentPos << 5 : currentPos << 4;
				uint32_t newSquare = capturedPiece & PLUS_5_MOVE_AVAILABLE ? capturedPiece << 5 : capturedPiece << 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push((currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare);
					furtherCaptures = true;
				}
			}

			// left up capture
			if (currentPos & LEFT_UP_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & PLUS_3_MOVE_AVAILABLE ? currentPos << 3 : currentPos << 4;
				uint32_t newSquare = capturedPiece & PLUS_3_MOVE_AVAILABLE ? capturedPiece << 3 : capturedPiece << 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push((currentCapture& ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare);
					furtherCaptures = true;
				}
			}

			// right down capture
			if (currentPos & RIGHT_DOWN_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & MINUS_3_MOVE_AVAILABLE ? currentPos >> 3 : currentPos >> 4;
				uint32_t newSquare = capturedPiece & MINUS_3_MOVE_AVAILABLE ? capturedPiece >> 3 : capturedPiece >> 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push((currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare);
					furtherCaptures = true;
				}
			}

			// left down capture
			if (currentPos & LEFT_DOWN_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & MINUS_5_MOVE_AVAILABLE ? currentPos >> 5 : currentPos >> 4;
				uint32_t newSquare = capturedPiece & MINUS_5_MOVE_AVAILABLE ? capturedPiece >> 5 : capturedPiece >> 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push((currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare);
					furtherCaptures = true;
				}
			}
		}
		if (!furtherCaptures)
			fullCaptures.push_back(currentCapture);
	}
	return fullCaptures;
}

CheckersVector<Move> MoveGenerator::GenerateMovesWithNotation(uint32_t whitePieces, uint32_t blackPieces, uint32_t promotedPieces, bool isWhiteTurn)
{
	uint32_t ourPieces = isWhiteTurn ? whitePieces : blackPieces;
	uint32_t enemyPieces = isWhiteTurn ? blackPieces : whitePieces;
	uint32_t emptySquares = ~(ourPieces | enemyPieces);

	CheckersVector<Move> nonCaptureMoves;
	CheckersQueue<Move> captureMoves;

	uint32_t ourQueens = ourPieces & promotedPieces;
	uint32_t ourPawns = ourPieces & ~ourQueens;

	while (ourQueens)
	{
		int square = countr_zero(ourQueens);
		uint32_t squareMask = 1u << square;
		string squareNotation = boardMap.at(squareMask);

		for (const auto& dir : QueenDirs)
		{
			uint32_t index = squareMask;
			uint32_t enemyPiecePassed = 0;

			while (index & dir.moveAvailableMask)
			{
				index = (index & dir.moveTypeMask) ? Shift(index, dir.shiftA) : Shift(index, dir.shiftB);
				string landingSquare = boardMap.at(index);

				if (index & ourPieces)
					break;
				else if (index & enemyPieces)
				{
					if (enemyPiecePassed) break;
					enemyPiecePassed = index;
				}
				else if (enemyPiecePassed)
				{
					captureMoves.push({ squareMask | enemyPiecePassed | index, squareNotation + ":" + landingSquare });
				}
				else
					nonCaptureMoves.push_back({ squareMask | index, squareNotation + "-" + landingSquare});
			}
		}

		ourQueens = ourQueens & (ourQueens - 1);
	}

	while (ourPawns)
	{
		int square = countr_zero(ourPawns);
		uint32_t squareMask = 1u << square;
		string squareNotation = boardMap.at(squareMask);

		uint32_t moves = emptySquares & (isWhiteTurn ? whitePawnMoveMasks[square] : blackPawnMoveMasks[square]);
		while (moves)
		{
			uint32_t landingSquare = 1u << countr_zero(moves);
			nonCaptureMoves.push_back({ squareMask | landingSquare,squareNotation + "-" + boardMap.at(landingSquare)});
			moves = moves & (moves - 1);
		}

		// right up capture
		if (squareMask & RIGHT_UP_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & PLUS_5_MOVE_AVAILABLE ? squareMask << 5 : squareMask << 4;
			uint32_t newSquare = capturedPieceSquare & PLUS_5_MOVE_AVAILABLE ? capturedPieceSquare << 5 : capturedPieceSquare << 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push({ squareMask | capturedPieceSquare | newSquare ,squareNotation + ":" + boardMap.at(newSquare)});
		}

		// left up capture
		if (squareMask & LEFT_UP_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & PLUS_3_MOVE_AVAILABLE ? squareMask << 3 : squareMask << 4;
			uint32_t newSquare = capturedPieceSquare & PLUS_3_MOVE_AVAILABLE ? capturedPieceSquare << 3 : capturedPieceSquare << 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push({ squareMask | capturedPieceSquare | newSquare ,squareNotation + ":" + boardMap.at(newSquare) });
		}

		// right down capture
		if (squareMask & RIGHT_DOWN_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & MINUS_3_MOVE_AVAILABLE ? squareMask >> 3 : squareMask >> 4;
			uint32_t newSquare = capturedPieceSquare & MINUS_3_MOVE_AVAILABLE ? capturedPieceSquare >> 3 : capturedPieceSquare >> 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push({ squareMask | capturedPieceSquare | newSquare ,squareNotation + ":" + boardMap.at(newSquare) });
		}

		// left down capture
		if (squareMask & LEFT_DOWN_CAPTURE_AVAILABLE)
		{
			uint32_t capturedPieceSquare = squareMask & MINUS_5_MOVE_AVAILABLE ? squareMask >> 5 : squareMask >> 4;
			uint32_t newSquare = capturedPieceSquare & MINUS_5_MOVE_AVAILABLE ? capturedPieceSquare >> 5 : capturedPieceSquare >> 4;
			if (capturedPieceSquare & enemyPieces && newSquare & emptySquares)
				captureMoves.push({ squareMask | capturedPieceSquare | newSquare ,squareNotation + ":" + boardMap.at(newSquare) });
		}

		ourPawns = ourPawns & (ourPawns - 1);
	}
	if (captureMoves.empty())
		return nonCaptureMoves;
	else
		return GenerateWholeCapturesWithNotation(ourPieces, enemyPieces, promotedPieces, captureMoves);
}

CheckersVector<Move> MoveGenerator::GenerateWholeCapturesWithNotation(uint32_t ourPieces, uint32_t enemyPieces, uint32_t promotedPieces, CheckersQueue<Move> captureQueue)
{
	CheckersVector<Move> fullCaptures;

	while (!captureQueue.empty())
	{
		bool furtherCaptures = false;
		uint32_t currentCapture = captureQueue.front().mask;
		string currentNotation = captureQueue.front().notation;
		captureQueue.pop();

		uint32_t capturableEnemyPieces = enemyPieces & ~(currentCapture);
		uint32_t currentPos = (currentCapture & (~(ourPieces | enemyPieces)));
		if (!currentPos) currentPos = currentCapture & ourPieces;
		uint32_t blockers = (enemyPieces & currentCapture) | (ourPieces & ~currentCapture);
		uint32_t freeSquares = (~(ourPieces | enemyPieces) | (ourPieces & currentCapture)) & (~currentPos);

		// queen captures
		if (promotedPieces & ourPieces & currentCapture)
		{
			for (const auto& dir : QueenDirs)
				GenerateQueenCaptureWithNotation(dir, currentPos, blockers, capturableEnemyPieces, currentCapture, ourPieces, enemyPieces, captureQueue, furtherCaptures, currentNotation);
		}
		// pawn captures
		else
		{
			// right up capture
			if (currentPos & RIGHT_UP_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & PLUS_5_MOVE_AVAILABLE ? currentPos << 5 : currentPos << 4;
				uint32_t newSquare = capturedPiece & PLUS_5_MOVE_AVAILABLE ? capturedPiece << 5 : capturedPiece << 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push({ (currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare , currentNotation + boardMap.at(newSquare)});
					furtherCaptures = true;
				}
			}

			// left up capture
			if (currentPos & LEFT_UP_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & PLUS_3_MOVE_AVAILABLE ? currentPos << 3 : currentPos << 4;
				uint32_t newSquare = capturedPiece & PLUS_3_MOVE_AVAILABLE ? capturedPiece << 3 : capturedPiece << 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push({ (currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare , currentNotation + boardMap.at(newSquare) });
					furtherCaptures = true;
				}
			}

			// right down capture
			if (currentPos & RIGHT_DOWN_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & MINUS_3_MOVE_AVAILABLE ? currentPos >> 3 : currentPos >> 4;
				uint32_t newSquare = capturedPiece & MINUS_3_MOVE_AVAILABLE ? capturedPiece >> 3 : capturedPiece >> 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push({ (currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare , currentNotation + boardMap.at(newSquare) });
					furtherCaptures = true;
				}
			}

			// left down capture
			if (currentPos & LEFT_DOWN_CAPTURE_AVAILABLE)
			{
				uint32_t capturedPiece = currentPos & MINUS_5_MOVE_AVAILABLE ? currentPos >> 5 : currentPos >> 4;
				uint32_t newSquare = capturedPiece & MINUS_5_MOVE_AVAILABLE ? capturedPiece >> 5 : capturedPiece >> 4;
				if (capturedPiece & capturableEnemyPieces && newSquare & freeSquares)
				{
					captureQueue.push({ (currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | newSquare , currentNotation + boardMap.at(newSquare) });
					furtherCaptures = true;
				}
			}
		}
		if (!furtherCaptures)
			fullCaptures.push_back({ currentCapture,currentNotation });
	}
	return fullCaptures;
}

void MoveGenerator::GenerateQueenCaptureWithNotation(const Direction& dir, uint32_t currentPos, uint32_t blockers, uint32_t capturableEnemyPieces, uint32_t currentCapture, uint32_t ourPieces, uint32_t enemyPieces, CheckersQueue<Move>& captureQueue, bool& furtherCaptures, std::string& currentNotation)
{
	uint32_t index = (currentPos & dir.moveAvailableMask) ? currentPos : 0;
	uint32_t capturedPiece = 0;
	while (index & dir.moveAvailableMask)
	{
		index = index & dir.moveTypeMask ? Shift(index, dir.shiftA) : Shift(index, dir.shiftB);
		;
		if (index & blockers)
			break;
		else if (index & capturableEnemyPieces)
		{
			if (capturedPiece) break;
			capturedPiece = index;
		}
		else if (capturedPiece)
		{
			captureQueue.push({ (currentCapture & ourPieces) | (enemyPieces & currentCapture) | capturedPiece | index , currentNotation + ":" + boardMap.at(index)});
			furtherCaptures = true;
		}
	}
}
