#pragma once

#include "EvaluationWeights.hpp"
class MinimaxPlayer;

struct Bot {
    EvaluationWeights weights;

    float score = 0.0f; 

    Bot() = default;

    explicit Bot(const EvaluationWeights& weights)
        : weights(weights) {
    }

    MinimaxPlayer CreatePlayer(bool isWhite, int depth = 200, int timeLimit = 30) const; 
};
