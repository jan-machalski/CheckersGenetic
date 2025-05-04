#pragma once  
#include "EvaluationWeights.hpp"  
#include "Board.h"  
#include "MoveGenerator.h"  

static constexpr uint32_t BAND_MASK = 0x81818181;
static constexpr uint32_t ROW_MASKS[8] = {
	0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
	0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F
};

struct SupportedPawnsInfo {
	float singlySupportedDiff;
	float doublySupportedDiff;
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
  float PromotionDistance(const Board& board) const;
  int FreePawnsCountDiff(const Board& board) const;

};
