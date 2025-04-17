#pragma once
#include <cstdint>
#include "Board.h"

class Judge
{
public:

    static inline bool IsMoveAdvancing(uint32_t moveMask, const Board& board) {
        uint32_t ourPieces = board.isWhiteTurn ? board.whitePieces : board.blackPieces;
        uint32_t opponentPieces = board.isWhiteTurn ? board.blackPieces : board.whitePieces;
        return (moveMask & opponentPieces) || (ourPieces & moveMask & board.promotedPieces);
    }

    static inline bool IsCheckmate(const Board& board) {
        auto moves = board.GenerateMoves();
        return moves.size() == 0;
    }

private:
	Judge() = delete; 
	Judge(const Judge&) = delete;
	Judge& operator=(const Judge&) = delete;
};