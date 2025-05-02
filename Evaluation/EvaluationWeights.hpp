#pragma once

class EvaluationWeights {
public:

    float piecesWeight = 1.0;    
    float kingsWeight = 3.0;     

    EvaluationWeights() {}

    EvaluationWeights(float piecesWeight, float kingsWeight) : piecesWeight(piecesWeight), kingsWeight(kingsWeight) {}
};