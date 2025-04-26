#include "MinimaxPlayer.hpp"
#include <limits>
#include <chrono>

#define PRINT_EVALUATIONS

uint32_t MinimaxPlayer::MakeMove()
{
	uint32_t bestMove = iterativeDeepening();

	if (bestMove)
	{
		board.ApplyMove(bestMove);
		transpositionTable.softClear();
	}
	
	return bestMove;
}

void MinimaxPlayer::InputMove(uint32_t moveMask)
{
	board.ApplyMove(moveMask);
	previousBestMove = 0;
	transpositionTable.softClear();
}


uint32_t MinimaxPlayer::iterativeDeepening()
{
	auto startTime = std::chrono::high_resolution_clock::now();

	uint32_t bestMove = 0;

	auto moves = board.GenerateMoves();

	if (moves.empty())
		return 0;

	if (moves.size() == 1)
	{
		return moves[0];
	}

	for (int depth = 1; depth <= maxDepth; depth++)
	{
		float bestValue = std::numeric_limits<float>::min();
		uint32_t currentBestMove = 0;

		float alpha = (depth >= 3) ? bestValue - 100 : std::numeric_limits<float>::min();
		float beta = (depth >= 3) ? bestValue + 100 : std::numeric_limits<float>::max();
		
		for (uint32_t move : moves)
		{
			auto tmpBoard = board.MakeMove(move);
			bool moveIsCapture = board.isCapture(move);
		
			uint32_t childBestMove = 0;
			float value = -alphaBeta(tmpBoard, depth - 1, -beta, -alpha, false, moveIsCapture, childBestMove);
		
			if (depth >= 3 && (value <= alpha || value >= beta)) {
				float fullAlpha = std::numeric_limits<float>::min();
				float fullBeta = std::numeric_limits<float>::max();
				value = -alphaBeta(tmpBoard, depth - 1, -fullBeta, -fullAlpha,false, moveIsCapture, childBestMove);
			}
			if (value > bestValue)
			{
				bestValue = value;
				currentBestMove = move;
                alpha = std::max(alpha, value);
			}
		}

		bestMove = currentBestMove;
		previousBestMove = bestMove;

#ifdef PRINT_EVALUATIONS
		printf( "G³êbokoœæ %d, najlepszy ruch: %ld, ocena: %f\n",depth,bestMove,bestValue);
#endif
		/*auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
		if (elapsedTime > timePerMove * 0.8) { 
			break;
		}*/
	}
	return bestMove;

}

float MinimaxPlayer::alphaBeta(const Board& board, int depth, float alpha, float beta, bool maximizingPlayer, bool isCapture, uint32_t& bestMove)
{
	uint64_t zobristHash = TranspositionTable::computeZobristHash(board);

	int storedDepth;
	float storedValue;
	TranspositionTable::NodeType storedType;
	uint32_t storedBestMove;

	if (transpositionTable.probe(zobristHash, depth, storedDepth, storedValue, storedType, storedBestMove))
	{
		if (storedType == TranspositionTable::NodeType::EXACT)
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
		return maximizingPlayer ? std::numeric_limits<float>::min() : std::numeric_limits<float>::max();

	if (depth <= 0 && !isCapture)
	{
		bestMove = 0;
		float evaluation = evaluator.Evaluate(board);
		return maximizingPlayer ? evaluation : -evaluation;
	}

	placeBestMove(moves);

	float originalAlpha = alpha;
	TranspositionTable::NodeType nodeType = TranspositionTable::NodeType::EXACT;
	float bestValue;
	bestMove = 0;

	if (maximizingPlayer)
	{
		bestValue = std::numeric_limits<float>::min();

		for (uint32_t move : moves)
		{
			auto tmpBoard = board.MakeMove(move);
			bool nextIsCapture = board.isCapture(move);

			int nextDepth = depth - 1;

			if (nextIsCapture && nextDepth < 0)
				nextDepth = 0;

			uint32_t childBestMove = 0;
			float value = alphaBeta(tmpBoard, nextDepth, alpha, beta, false,nextIsCapture ,childBestMove);
		
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
	}
	else
	{
		bestValue = std::numeric_limits<float>::max();

		for (uint32_t move : moves)
		{
			Board tmpBoard = board.MakeMove(move);
			bool nextIsCapture = board.isCapture(move);

			int nextDepth = depth - 1;

			if (nextIsCapture && nextDepth < 0)
				nextDepth = 0;

			uint32_t childBestMove = 0;
			float value = alphaBeta(tmpBoard, nextDepth, alpha, beta, true,nextIsCapture, childBestMove);
		
			if (value < bestValue)
			{
				bestValue = value;
				bestMove = move;
				if (value < beta)
				{
					beta = value;
				}
			}
			if (alpha >= beta)
			{
				nodeType = TranspositionTable::NodeType::UPPERBOUND;
				break;
			}
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