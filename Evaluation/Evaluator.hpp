#pragma once
#include "EvaluationWeights.hpp"
#include "Board.h"

class Evaluator
{
private:
	EvaluationWeights weights;

public:
	Evaluator(const EvaluationWeights& weights) : weights(weights) {}
	Evaluator() : weights(EvaluationWeights()) {}

	float Evaluate(const Board& board) const;

private:
	int PawnsCountDiff(const Board& board) const;
	int KingsCountDiff(const Board& board) const;
};

