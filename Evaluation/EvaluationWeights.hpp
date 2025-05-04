#pragma once

#include <array>

enum class Heuristic : std::size_t {
    PAWN_COUNT,
	KING_COUNT,
    MOBILITY,
    COUNT
};

class EvaluationWeights {
public:

    std::array<float, static_cast<std::size_t>(Heuristic::COUNT)> weights;

    EvaluationWeights() {
		weights.fill(0.0f);
		weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)] = 1.0f;
		weights[static_cast<std::size_t>(Heuristic::KING_COUNT)] = 3.0f;
    }
	EvaluationWeights(const EvaluationWeights& other) {
		weights = other.weights;
	}
	EvaluationWeights(float piecesWeight, float kingsWeight, float mobility) {
		weights.fill(0.0f);
		weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)] = piecesWeight;
		weights[static_cast<std::size_t>(Heuristic::KING_COUNT)] = kingsWeight;
		weights[static_cast<std::size_t>(Heuristic::MOBILITY)] = mobility;
	}

};