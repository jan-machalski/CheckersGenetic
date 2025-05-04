#include "Evaluator.hpp"

float Evaluator::Evaluate(const Board& board) const
{
	float score = 0;
	score += PawnsCountDiff(board) * weights.weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)];
	score += KingsCountDiff(board) * weights.weights[static_cast<std::size_t>(Heuristic::KING_COUNT)];
	score += Mobility(board) * weights.weights[static_cast<std::size_t>(Heuristic::MOBILITY)];
	return board.isWhiteTurn ? score : -score;
}

int Evaluator::PawnsCountDiff(const Board& board) const
{
	return std::popcount(board.whitePieces & (~board.promotedPieces)) - std::popcount(board.blackPieces & (~board.promotedPieces));
}

int Evaluator::KingsCountDiff(const Board& board) const
{
	return std::popcount(board.whitePieces & board.promotedPieces) - std::popcount(board.blackPieces & board.promotedPieces);
}
int Evaluator::Mobility(const Board& board) const
{
	auto boardCopy = board;
	boardCopy.isWhiteTurn = true;
	auto whiteMoves = boardCopy.GenerateMoves().size();
	boardCopy.isWhiteTurn = false;
	auto blackMoves = boardCopy.GenerateMoves().size();
	return whiteMoves - blackMoves;
}