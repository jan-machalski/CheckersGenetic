#include "TranspositionTable.hpp"
#include "Evaluator.hpp"
#include "Player.hpp"

class MinimaxPlayer : public Player
{
private:
	Evaluator evaluator;

	TranspositionTable transpositionTable;

	const uint32_t timePerMove;
	const int maxDepth;

	uint32_t previousBestMove = 0;

	float alphaBeta(const Board& board, int depth, float alpha, float beta, bool maximizingPlayer, bool isCapture, uint32_t& bestMove);
	void placeBestMove(CheckersVector<uint32_t>& moves);

    uint32_t iterativeDeepening();
  

public:
    MinimaxPlayer(bool isWhite, uint32_t timePerMove, int maxDepth = 20)
        : Player(isWhite), evaluator(), timePerMove(timePerMove), maxDepth(maxDepth),
        transpositionTable(64) {
        static bool zobristInitialized = false;
        if (!zobristInitialized) {
            TranspositionTable::initZobristKeys();
            zobristInitialized = true;
        }
    }

    MinimaxPlayer(bool isWhite, uint32_t timePerMove, const EvaluationWeights& weights, int maxDepth = 20)
        : Player(isWhite), evaluator(weights), timePerMove(timePerMove), maxDepth(maxDepth),
        transpositionTable(64) {
        static bool zobristInitialized = false;
        if (!zobristInitialized) {
            TranspositionTable::initZobristKeys();
            zobristInitialized = true;
        }
    }

	uint32_t MakeMove() override;
	void InputMove(uint32_t moveMask) override;

};