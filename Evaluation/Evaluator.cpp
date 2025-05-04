#include "Evaluator.hpp"
#include "PromotionMasks.hpp"

float Evaluator::Evaluate(const Board& board) const
{
	float score = 0;
	score += PawnsCountDiff(board) * weights.weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)];
	score += KingsCountDiff(board) * weights.weights[static_cast<std::size_t>(Heuristic::KING_COUNT)];
	score += Mobility(board) * weights.weights[static_cast<std::size_t>(Heuristic::MOBILITY)];

	auto supportInfo = SupportedPawnsCountDiff(board);
	score += supportInfo.singlySupportedDiff * weights.weights[static_cast<std::size_t>(Heuristic::SINGLE_SUPPORTED)];
	score += supportInfo.doublySupportedDiff * weights.weights[static_cast<std::size_t>(Heuristic::DOUBLY_SUPPORTED)];

	score += PromotionDistance(board) * weights.weights[static_cast<std::size_t>(Heuristic::PROMOTION_DISTANCE)];
	score += FreePawnsCountDiff(board) * weights.weights[static_cast<std::size_t>(Heuristic::FREE_PAWNS)];

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

SupportedPawnsInfo Evaluator::SupportedPawnsCountDiff(const Board& board) const
{
	SupportedPawnsInfo result = { 0, 0 };

	uint32_t whitePawns = board.whitePieces & (~board.promotedPieces);
	uint32_t blackPawns = board.blackPieces & (~board.promotedPieces);

	float blackPawnsCount = float(std::popcount(blackPawns));
	float whitePawnsCount = float(std::popcount(whitePawns));

	uint32_t whiteSupported = 0;
	uint32_t whiteDoublySupported = 0;

	uint32_t blackSupported = 0;
	uint32_t blackDoublySupported = 0;

	while (whitePawns)
	{
		int squareIndex = std::countr_zero(whitePawns);
		whitePawns &= ~(1u << squareIndex);
		uint32_t supportedMask = blackPawnMoveMasks[squareIndex] & board.whitePieces;
		if (supportedMask)
		{
			whiteSupported |= supportedMask;
			int supports = std::popcount(supportedMask) + (((1u << squareIndex) & BAND_MASK) ? 1 : 0);
			if (supports == 2)
				whiteDoublySupported |= supportedMask;
		}
	}
	while (blackPawns)
	{
		int squareIndex = std::countr_zero(blackPawns);
		blackPawns &= ~(1u << squareIndex);
		uint32_t supportedMask = whitePawnMoveMasks[squareIndex] & board.blackPieces;
		if (supportedMask)
		{
			blackSupported |= supportedMask;
			int supports = std::popcount(supportedMask) + (((1u << squareIndex) & BAND_MASK) ? 1 : 0);
			if (supports == 2)
				blackDoublySupported |= supportedMask;
		}
	}

	result.singlySupportedDiff = std::popcount(whiteSupported)/whitePawnsCount - std::popcount(blackSupported)/blackPawnsCount;
	result.doublySupportedDiff = std::popcount(whiteDoublySupported)/whitePawnsCount - std::popcount(blackDoublySupported)/blackPawnsCount;
	return result;
}

float Evaluator::PromotionDistance(const Board& board) const
{
	int whiteDistance = 0;
	int blackDistance = 0;

	uint32_t whitePawns = board.whitePieces & (~board.promotedPieces);
	uint32_t blackPawns = board.blackPieces & (~board.promotedPieces);

	for (int i = 0; i < 8; i++)
	{
		whiteDistance += std::popcount(whitePawns & ROW_MASKS[i]) * (i);
		blackDistance += std::popcount(blackPawns & ROW_MASKS[i]) * (7 - i);
	}

	float whiteResult = whitePawns ? whiteDistance/std::popcount(whitePawns) : 0;
	float blackResult = blackPawns ? blackDistance / std::popcount(blackPawns) : 0;

	return whiteResult - blackResult;
}

int Evaluator::FreePawnsCountDiff(const Board& board) const
{
	uint32_t whitePawns = board.whitePieces & (~board.promotedPieces);
	uint32_t blackPawns = board.blackPieces & (~board.promotedPieces);
	int whiteFreePawns = 0;
	int blackFreePawns = 0;

	if(!(board.blackPieces & board.promotedPieces))
	{
		while (whitePawns)
		{
			int squareIndex = std::countr_zero(whitePawns);
			whitePawns &= ~(1u << squareIndex);
			if (!(PROMOTION_PATHS_WHITE[squareIndex] & blackPawns))
				whiteFreePawns++;
		}
	}

	if(!(board.whitePieces & board.promotedPieces))
	{
		while (blackPawns)
		{
			int squareIndex = std::countr_zero(blackPawns);
			blackPawns &= ~(1u << squareIndex);
			if (!(PROMOTION_PATHS_BLACK[squareIndex] & whitePawns))
				blackFreePawns++;
		}
	}
	while (blackPawns)
	{
		int squareIndex = std::countr_zero(blackPawns);
		blackPawns &= ~(1u << squareIndex);
		if (!(whitePawnMoveMasks[squareIndex] & board.whitePieces))
			blackFreePawns++;
	}
	return whiteFreePawns - blackFreePawns;
}



void Evaluator::InitMoveMasks() {
	static bool initialized = false; // Flaga, aby upewniæ siê, ¿e inicjalizacja jest wykonywana tylko raz
	if (!initialized) {
		for (int i = 0; i < 32; ++i) {
			whitePawnMoveMasks[i] = MoveGenerator::GenerateWhitePawnMoveMask(i);
			blackPawnMoveMasks[i] = MoveGenerator::GenerateBlackPawnMoveMask(i);
		}
		initialized = true;
	}
}