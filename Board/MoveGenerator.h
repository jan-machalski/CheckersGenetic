#pragma once
#include <cstdint>
#include <bit>
#include "CheckersVector.hpp"
#include "CheckersQueue.hpp"
#include "BoardMap.hpp"
#include "MoveDirections.hpp"

constexpr uint32_t RIGHT_UP_CAPTURE_AVAILABLE = 0x00777777;
constexpr uint32_t LEFT_UP_CAPTURE_AVAILABLE = 0x00EEEEEE;
constexpr uint32_t RIGHT_DOWN_CAPTURE_AVAILABLE = 0x77777700;
constexpr uint32_t LEFT_DOWN_CAPTURE_AVAILABLE = 0xEEEEEE00;

constexpr uint32_t RIGHT_UP_MOVE_AVAILABLE = 0x0F7F7F7F;
constexpr uint32_t LEFT_UP_MOVE_AVAILABLE = 0x0EFEFEFE;
constexpr uint32_t RIGHT_DOWN_MOVE_AVAILABLE = 0x7F7F7F70;
constexpr uint32_t LEFT_DOWN_MOVE_AVAILABLE = 0xFEFEFEF0;

constexpr uint32_t PLUS_5_MOVE_AVAILABLE = 0x00707070;
constexpr uint32_t PLUS_3_MOVE_AVAILABLE = 0x0E0E0E0E;
constexpr uint32_t PLUS_4_MOVE_AVAILABLE = 0x0FFFFFFF;
constexpr uint32_t MINUS_5_MOVE_AVAILABLE = 0x0E0E0E00;
constexpr uint32_t MINUS_3_MOVE_AVAILABLE = 0x70707070;
constexpr uint32_t MINUS_4_MOVE_AVAILABLE = 0xFFFFFFF0;

typedef struct Move {
	uint32_t mask;
	std::string notation;
}Move;

class MoveGenerator {
public:
	static void Init();

	static  CheckersVector<uint32_t> GenerateMoves(uint32_t whitePieces, uint32_t blackPieces, uint32_t promotedPieces, bool isWhiteTurn);
	static CheckersVector<Move> GenerateMovesWithNotation(uint32_t whitePieces, uint32_t blackPieces, uint32_t promotedPieces, bool isWhiteTurn);

	static uint32_t GenerateWhitePawnMoveMask(int squareIndex);
	static uint32_t GenerateBlackPawnMoveMask(int squareIndex);

private:

	static inline uint32_t whitePawnMoveMasks[32];
	static inline uint32_t blackPawnMoveMasks[32];
	static inline uint32_t rightUpSquareMasks[32];
	static inline uint32_t leftUpSquareMasks[32];
	static inline uint32_t rightDownSquareMasks[32];
	static inline uint32_t leftDownSquareMasks[32];
	
	static uint32_t GenerateRightUpSquareMask(int squareIndex);
	static uint32_t GenerateLeftUpSquareMask(int squareIndex);
	static uint32_t GenerateRightDownSquareMask(int squareIndex);
	static uint32_t GenerateLeftDownSquareMask(int squareIndex);

	static void GenerateQueenCapture(const Direction& dir, uint32_t currentPos, uint32_t blockers, uint32_t capturableEnemyPieces, uint32_t currentCapture, uint32_t ourPieces, uint32_t enemyPieces, CheckersQueue<uint32_t>& captureQueue, bool& furtherCaptures);
	static void GenerateQueenCaptureWithNotation(const Direction& dir, uint32_t currentPos, uint32_t blockers, uint32_t capturableEnemyPieces, uint32_t currentCapture, uint32_t ourPieces, uint32_t enemyPieces, CheckersQueue<Move>& captureQueue, bool& furtherCaptures, std::string& currentNotation);

	static CheckersVector<uint32_t> GenerateWholeCaptures(uint32_t ourPieces, uint32_t enemyPieces, uint32_t promotedPieces, CheckersQueue<uint32_t> captureQueue);
	static CheckersVector<Move> GenerateWholeCapturesWithNotation(uint32_t ourPieces, uint32_t enemyPieces, uint32_t promotedPieces, CheckersQueue<Move> captureQueue);


};