#pragma once  
#include "EvaluationWeights.hpp"  
#include "Board.h"  
#include "MoveGenerator.h"  

static constexpr uint32_t BAND_MASK = 0x81818181;

struct SupportedPawnsInfo {
	int singlySupportedDiff;
	int doublySupportedDiff;
};

class Evaluator  
{  
private:  
  EvaluationWeights weights;  
  static inline uint32_t whitePawnMoveMasks[32];
  static inline uint32_t blackPawnMoveMasks[32];

public:  
  Evaluator(const EvaluationWeights& weights) : weights(weights) { InitMoveMasks(); }
  Evaluator() : weights(EvaluationWeights()) { InitMoveMasks(); }

  float Evaluate(const Board& board) const;  

private:  
  static void InitMoveMasks();
  int PawnsCountDiff(const Board& board) const;  
  int KingsCountDiff(const Board& board) const;  
  int Mobility(const Board& board) const;  
  SupportedPawnsInfo SupportedPawnsCountDiff(const Board& board) const;
};
