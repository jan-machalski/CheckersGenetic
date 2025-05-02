#pragma once

#include "Player.hpp"

class HumanPlayer : public Player
{
public:
	HumanPlayer(bool isWhite) :Player(isWhite) {}

	uint32_t MakeMove() override;
	void InputMove(uint32_t moveMask) override;
};