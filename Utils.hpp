#pragma once
#include <cstdint>
#include <iostream>

inline void SetBit(uint32_t& value, int position, bool bit) {
	if (bit) {
		value |= (1 << position);
	}
	else {
		value &= ~(1 << position);
	}
}

inline uint32_t Shift(uint32_t val, int shift) {
	return shift > 0 ? val << shift : val >> -shift;
}

inline void printBoard(uint32_t playerPieces, uint32_t opponentPieces, uint32_t promotedPieces, bool whiteSideDown = true) {
	char columns[] = " | A| B| C| D| E| F| G| H| ";
	if (!whiteSideDown)
		std::reverse(std::begin(columns), std::end(columns) - 1);
	const char* separator = "-+--+--+--+--+--+--+--+--+-";

	printf("\n\t%s\n", columns);
	printf("\t%s\n", separator);

	for (int i = 0; i < 8; i++) {
		printf("\t%c|", whiteSideDown ? '8' - i : '1' + i);

		for (int j = 0; j < 4; j++) {
			if (i % 2 == 0) {
				printf("  |");
			}

			int position = (7-i) * 4 + j;

			if (playerPieces & (1 << position)) {
				printf("%c", whiteSideDown ? 'W' : 'B');
				if (promotedPieces & (1 << position))
					printf("%c", whiteSideDown ? 'W' : 'B');
				else
					printf(" ");
			}
			else if (opponentPieces & (1 << position)) {
				printf("%c", whiteSideDown ? 'B' : 'W');
				if (promotedPieces & (1 << position))
					printf("%c", whiteSideDown ? 'B' : 'W');
				else
					printf(" ");
			}
			else {
				printf("  ");
			}

			if (i % 2 == 0) {
				printf("|");
			}
			else {
				printf("|  |");
			}
		}

		printf("%c\n", whiteSideDown ? '8' - i : '1' + i);
		printf("\t%s\n", separator);
	}

	printf("\t%s\n\n", columns);
}