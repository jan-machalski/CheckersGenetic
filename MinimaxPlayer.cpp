#include "MinimaxPlayer.hpp"
#include <limits>
#include <chrono>
#include "Utils.hpp"

#define PRINT_EVALS

uint32_t MinimaxPlayer::MakeMove()
{
	uint32_t bestMove = iterativeDeepening();

	if (bestMove)
	{
		board.ApplyMove(bestMove);
		transpositionTable.clear();
	}
	
	return bestMove;
}

void MinimaxPlayer::InputMove(uint32_t moveMask)
{
	board.ApplyMove(moveMask);
	previousBestMove = 0;
	transpositionTable.clear();
}


uint32_t MinimaxPlayer::iterativeDeepening()
{
	auto startTime = std::chrono::high_resolution_clock::now();

	auto moves = board.GenerateMoves();

	if (moves.empty())
		return 0;

	uint32_t bestMove = moves[0];

	if (moves.size() == 1)
	{
		return moves[0];
	}

	for (int depth = 1; depth <= maxDepth; depth++)
	{
		float bestValue = -BIG_NUMBER;
		uint32_t currentBestMove = 0;

		float alpha =  -BIG_NUMBER;
		float beta =  BIG_NUMBER;
		
		for (uint32_t move : moves)
		{
			auto tmpBoard = board.MakeMove(move);
			bool moveIsCapture = board.isCapture(move);
		
			uint32_t childBestMove = 0;
			float value = -alphaBeta(tmpBoard, depth - 1, -beta, -alpha, false, false, childBestMove);
		
			if (value > bestValue)
			{
				bestValue = value;
				currentBestMove = move;
                alpha = std::max(alpha, value);
			}
		}

		if (currentBestMove != 0) {
			bestMove = currentBestMove;
			previousBestMove = bestMove;
		}
#ifdef PRINT_EVALS
		printf("Depth %d, best move: %ld, evaluation: %f\n", depth, bestMove, bestValue);
#endif // PRINT_EVALS

		

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
		if (elapsedTime > timePerMove * 0.8) { 
			break;
		}
	}
	return bestMove;

}

float MinimaxPlayer::alphaBeta(const Board& board, int depth, float alpha, float beta, bool maximizingPlayer,bool calledRecursively, uint32_t& bestMove)
{
	uint64_t zobristHash = TranspositionTable::computeZobristHash(board);

	int storedDepth;
	float storedValue;
	TranspositionTable::NodeType storedType;
	uint32_t storedBestMove;

	if (transpositionTable.probe(zobristHash, depth, storedDepth, storedValue, storedType, storedBestMove))
	{
		if (storedDepth >= depth && storedType == TranspositionTable::NodeType::EXACT)
			return storedValue;
		else if (storedType == TranspositionTable::NodeType::LOWERBOUND)
			alpha = std::max(alpha, storedValue);
		else if (storedType == TranspositionTable::NodeType::UPPERBOUND)
			beta = std::min(beta, storedValue);
		if (alpha >= beta)
			return storedValue;
	}

	auto moves = board.GenerateMoves();

	if(moves.empty())
		return static_cast<float>(-BIG_NUMBER);

	if (depth <= 0)
	{
		bestMove = 0;
		float evaluation = evaluator.Evaluate(board);
		return evaluation;
	}

	if(!calledRecursively)
		placeBestMove(moves);

	float originalAlpha = alpha;
	TranspositionTable::NodeType nodeType = TranspositionTable::NodeType::EXACT;
	float bestValue;
	bestMove = 0;

	bestValue = -BIG_NUMBER;

	for (uint32_t move : moves)
	{
		auto tmpBoard = board.MakeMove(move);
		bool nextIsCapture = board.isCapture(move);

		int nextDepth = depth - 1;

		if (nextIsCapture && nextDepth < 0)
			nextDepth = 0;

		uint32_t childBestMove = 0;
		float value = -alphaBeta(tmpBoard, nextDepth, -beta, -alpha, false,true, childBestMove);

		if (value > bestValue)
		{
			bestValue = value;
			bestMove = move;

			if (value > alpha)
			{
				alpha = value;
			}
		}
		if (alpha >= beta)
		{
			nodeType = TranspositionTable::NodeType::LOWERBOUND;
			break;
		}
	}
	
	
	if (bestValue <= originalAlpha)
		nodeType = TranspositionTable::NodeType::UPPERBOUND;
	else if (bestValue >= beta)
		nodeType = TranspositionTable::NodeType::LOWERBOUND;
	else
		nodeType = TranspositionTable::NodeType::EXACT;

	transpositionTable.store(zobristHash, depth, bestValue, nodeType, bestMove);
	return bestValue;
}
void MinimaxPlayer::placeBestMove(CheckersVector<uint32_t>& moves)
{
	if (previousBestMove != 0) {
		auto it = std::find(moves.begin(), moves.end(), previousBestMove);
		if (it != moves.end()) {
			std::swap(*it, *moves.begin());
		}
	}
}