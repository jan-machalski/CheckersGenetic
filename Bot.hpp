#pragma once

#include "EvaluationWeights.hpp"
// Forward declare MinimaxPlayer
class MinimaxPlayer;

struct Bot {
    EvaluationWeights weights;

    float score = 0.0f; // Tournament score

    Bot() = default;

    explicit Bot(const EvaluationWeights& weights)
        : weights(weights) {
    }

    // Optionally create a player directly
    MinimaxPlayer CreatePlayer(bool isWhite, int depth = 200, int timeLimit = 30) const;
};
