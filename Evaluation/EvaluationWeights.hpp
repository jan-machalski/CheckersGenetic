#pragma once

#include <array>

enum class Heuristic : std::size_t {
    PAWN_COUNT,
	KING_COUNT,
    MOBILITY,
	SINGLE_SUPPORTED,
	DOUBLY_SUPPORTED,
	PROMOTION_DISTANCE,
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
	EvaluationWeights(float piecesWeight, float kingsWeight, float mobility, float singleSupported, float doublySupported, float promotionDistance) {
		weights.fill(0.0f);
		weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)] = piecesWeight;
		weights[static_cast<std::size_t>(Heuristic::KING_COUNT)] = kingsWeight;
		weights[static_cast<std::size_t>(Heuristic::MOBILITY)] = mobility;
		weights[static_cast<std::size_t>(Heuristic::SINGLE_SUPPORTED)] = singleSupported;
		weights[static_cast<std::size_t>(Heuristic::DOUBLY_SUPPORTED)] = doublySupported;
		weights[static_cast<std::size_t>(Heuristic::PROMOTION_DISTANCE)] = promotionDistance;
	}
	void Print() const {
		printf(" Pieces = % .2f, Kings = % .2f, Mobility = % .2f, Single supported = % .2f, Doubly supported = % .2f, Promotion distance = % .2f\n",
			weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)],
			weights[static_cast<std::size_t>(Heuristic::KING_COUNT)],
			weights[static_cast<std::size_t>(Heuristic::MOBILITY)],
			weights[static_cast<std::size_t>(Heuristic::SINGLE_SUPPORTED)],
			weights[static_cast<std::size_t>(Heuristic::DOUBLY_SUPPORTED)],
			weights[static_cast<std::size_t>(Heuristic::PROMOTION_DISTANCE)]);
	}

};