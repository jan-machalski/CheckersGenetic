#pragma once
#include "Player.hpp"

class MonteCarloPlayer : public Player
{
public:
	struct TreeNode
	{
		TreeNode* parent;
		CheckersVector<TreeNode*> children;

		Board board;
		uint64_t gamesPlayed = 0;
		uint64_t totalPoints = 0; // 2 for the win 1 for the draw 0 for the loss

		TreeNode(Board board, TreeNode* parent) : board(board), parent(parent) {}
	};

	const uint32_t timePerMove;
	TreeNode* root = nullptr;

	MonteCarloPlayer(bool isWhite, uint32_t timePerMove) : Player(isWhite), timePerMove(timePerMove) {
		root = new TreeNode(board, nullptr);
	}

	~MonteCarloPlayer() {
		DeleteNode(root);
	}

	uint32_t MakeMove() override;
	void InputMove(uint32_t moveMask) override;

private:
	void DeleteNode(TreeNode* node);

	uint8_t Simulation(Board board);
	TreeNode* Selection();
	TreeNode* Expansion(TreeNode* node);
	void BackPropagation(TreeNode* node, uint8_t result);
	void PerformMC();
};