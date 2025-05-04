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
    };

    explicit TranspositionTable(size_t sizeInMB = 16);
    TranspositionTable(const TranspositionTable& other) {
        size = other.size;
        table = new Entry[size];
        std::memcpy(table, other.table, size * sizeof(Entry));
    }

    TranspositionTable& operator=(const TranspositionTable& other) {
        if (this != &other) {
            delete[] table;
            size = other.size;
            table = new Entry[size];
            std::memcpy(table, other.table, size * sizeof(Entry));
        }
        return *this;
    }

    void store(uint64_t zobristHash, int depth, float value, NodeType type, uint32_t bestMove);
    bool probe(uint64_t zobristHash, int requiredDepth, int& outDepth, float& outValue, NodeType& outType, uint32_t& outBestMove);
    void clear();

    static void initZobristKeys();
    static uint64_t computeZobristHash(const Board& board);

    ~TranspositionTable();

private:

    Entry* table = nullptr;
    size_t size;

    static uint64_t zobristPieces[2][32];
    static uint64_t zobristKings[2][32];
    static uint64_t zobristTurn;
    size_t getPrimeSize(size_t n);
};
