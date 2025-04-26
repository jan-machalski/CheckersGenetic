#pragma once

#include "Board.h"
#include <vector>
#include <cstdint>

class TranspositionTable {
public:
    enum class NodeType {
        EXACT,
        LOWERBOUND,
        UPPERBOUND
    };

    struct Entry {
        uint64_t zobristHash = 0;
        int depth = -1;
        float value = 0;
        NodeType type = NodeType::EXACT;
        uint32_t bestMove = 0;
        uint16_t age = 0;
    };

    explicit TranspositionTable(size_t sizeInMB = 64);

    void store(uint64_t zobristHash, int depth, float value, NodeType type, uint32_t bestMove);
    bool probe(uint64_t zobristHash, int requiredDepth, int& outDepth, float& outValue, NodeType& outType, uint32_t& outBestMove);
    void clear();
	void softClear();
    void incrementAge();

    static void initZobristKeys();
    static uint64_t computeZobristHash(const Board& board);

private:
    std::vector<Entry> table;
    size_t size;
    uint16_t currentAge = 0;

    static uint64_t zobristPieces[2][32];
    static uint64_t zobristKings[2][32];
    static uint64_t zobristTurn;
    size_t getPrimeSize(size_t n);
};
