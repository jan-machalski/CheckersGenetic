#include "TranspositionTable.hpp"

#include <random>

uint64_t TranspositionTable::zobristPieces[2][32];
uint64_t TranspositionTable::zobristKings[2][32];
uint64_t TranspositionTable::zobristTurn;

TranspositionTable::TranspositionTable(size_t sizeInMB) {
	size_t entries = (sizeInMB * 1024 * 1024) / sizeof(Entry);

	size = getPrimeSize(entries);

    table.resize(size);
    clear();
}

void TranspositionTable::store(uint64_t zobristHash, int depth, float value, NodeType type, uint32_t bestMove) {
    size_t index = zobristHash % size;
    Entry& entry = table[index];

    if (entry.age < currentAge || entry.depth <= depth) {
        entry.zobristHash = zobristHash;
        entry.depth = depth;
        entry.value = value;
        entry.type = type;
        entry.bestMove = bestMove;
        entry.age = currentAge;
    }
}

bool TranspositionTable::probe(uint64_t zobristHash, int requiredDepth, int& outDepth, float& outValue, NodeType& outType, uint32_t& outBestMove) {
    size_t index = zobristHash % size;
    Entry& entry = table[index];

    if (entry.zobristHash == zobristHash && entry.depth >= requiredDepth) {
        outDepth = entry.depth;
        outValue = entry.value;
        outType = entry.type;
        outBestMove = entry.bestMove;
        return true;
    }
    return false;
}

void TranspositionTable::clear() {
    std::fill(table.begin(), table.end(), Entry{});
    currentAge = 0;
}

void TranspositionTable::softClear() {
    incrementAge();
}

void TranspositionTable::incrementAge() {
    currentAge++;
}

void TranspositionTable::initZobristKeys() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (int color = 0; color < 2; ++color) {
        for (int square = 0; square < 32; ++square) {
            zobristPieces[color][square] = dis(gen);
            zobristKings[color][square] = dis(gen);
        }
    }
    zobristTurn = dis(gen);
}

uint64_t TranspositionTable::computeZobristHash(const Board& board) {
    uint64_t hash = 0;

    uint32_t white = board.whitePieces;
    uint32_t black = board.blackPieces;
    uint32_t kings = board.promotedPieces;

    for (int i = 0; i < 32; ++i) {
        uint32_t mask = 1u << i;
        if (white & mask) {
            hash ^= (kings & mask) ? zobristKings[0][i] : zobristPieces[0][i];
        }
        else if (black & mask) {
            hash ^= (kings & mask) ? zobristKings[1][i] : zobristPieces[1][i];
        }
    }

    if (board.isWhiteTurn)
        hash ^= zobristTurn;

    return hash;
}

size_t TranspositionTable::getPrimeSize(size_t n) {
    static const size_t primes[] = {
        1009, 2003, 4001, 8009, 16001, 32003, 64007, 128021,
        256019, 512009, 1024021, 2048003, 4096013, 8192003,
        16384001, 32768011, 65536007, 131072003, 262144009
    };

    for (size_t prime : primes) {
        if (prime >= n) return prime;
    }

    return n; 
}