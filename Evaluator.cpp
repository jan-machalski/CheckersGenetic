#include "Evaluator.hpp"

float Evaluator::Evaluate(const Board& board) const
{
	float score = 0;
	score += PawnsCountDiff(board) * weights.piecesWeight;
	score += KingsCountDiff(board) * weights.kingsWeight;
	return board.isWhiteTurn ? score : -score;
}

int Evaluator::PawnsCountDiff(const Board& board) const
{
	return std::popcount(board.whitePieces) - std::popcount(board.blackPieces);
}

int Evaluator::KingsCountDiff(const Board& board) const
{
	return std::popcount(board.whitePieces & board.promotedPieces) - std::popcount(board.blackPieces & board.promotedPieces);
}