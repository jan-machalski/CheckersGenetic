#pragma once
#include <cstdint>

struct Direction {
    uint32_t moveAvailableMask;
    uint32_t moveTypeMask;
    int shiftA;
    int shiftB;
};

constexpr Direction QueenDirs[] = {
    {0x0F7F7F7F, 0x00707070, 5, 4},
    {0x0EFEFEFE, 0x0E0E0E0E, 3, 4},
    {0x7F7F7F70, 0x70707070, -3, -4},
    {0xFEFEFEF0, 0x0E0E0E00, -5, -4}
}; 