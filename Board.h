#pragma once
#include <vector>
#include "MoveGenerator.h"


constexpr uint32_t WHITE_PIECES_INIT = 0x00000FFF;
constexpr uint32_t BLACK_PIECES_INIT = 0xFFF00000;
constexpr uint32_t PROMOTED_PIECES_INIT = 0u;
constexpr uint32_t WHITE_PROMOTION_MASK = 0xF0000000;
constexpr uint32_t BLACK_PROMOTION_MASK = 0x0000000F;

constexpr uint8_t MAX_NON_ADVANCING_MOVE_COUNT = 10; // half moves, not full moves

class Board {
public:
	uint32_t whitePieces;
	uint32_t blackPieces;
	uint32_t promotedPieces;

	bool isWhiteTurn;
	uint8_t nonAdvancingMoveCount = 0;


	Board(uint32_t whitePieces, uint32_t blackPieces, uint32_t promotedPieces,  uint32_t isWhiteTurn = true, uint8_t nonAdvancingMoveCount = 0) :
		whitePieces(whitePieces), blackPieces(blackPieces), promotedPieces(promotedPieces),nonAdvancingMoveCount(nonAdvancingMoveCount), isWhiteTurn(isWhiteTurn) {
	}

	Board(): whitePieces(WHITE_PIECES_INIT),blackPieces(BLACK_PIECES_INIT),promotedPieces(PROMOTED_PIECES_INIT),isWhiteTurn(true){}
	Board(bool isWhite) : whitePieces(WHITE_PIECES_INIT), blackPieces(BLACK_PIECES_INIT), promotedPieces(PROMOTED_PIECES_INIT), isWhiteTurn(isWhite) {}



	CheckersVector<uint32_t> GenerateMoves() const;
	CheckersVector<Move> GenerateMovesWithNotation() const;

	Board MakeMove(uint32_t moveMaks);
	void ApplyMove(uint32_t moveMask);

	void Print();
};


