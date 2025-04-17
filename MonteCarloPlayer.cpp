#include "MonteCarloPlayer.hpp"
#include "Utils.hpp"

void MonteCarloPlayer::DeleteNode(TreeNode* node)
{
	if (node == nullptr) return;
	for (auto child : node->children)
	{
		DeleteNode(child);
	}
}
uint8_t MonteCarloPlayer::Simulation(Board board)
{
	bool isWhiteNode = board.isWhiteTurn;
	while (true)
	{
		if (board.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
			return 1; // draw

		auto moves = board.GenerateMoves();

		if (moves.size() == 0)
			return board.isWhiteTurn == isWhiteNode ? 0 : 2; // 0 - loss, 2 - win


		uint8_t randomMove = rand() % moves.size();

		board.ApplyMove(moves[randomMove]);
	}
	return 1;
}

MonteCarloPlayer::TreeNode* MonteCarloPlayer::Selection()
{
	TreeNode* p = root;
	TreeNode* bestNode = nullptr;
	double bestScore = 0;

	while (!((p->children).empty()))
	{
		for (auto child : p->children)
		{
			if (child->gamesPlayed == 0)
			{
				bestNode = child;
				break;
			}

			double score = 2 * child->gamesPlayed - child->totalPoints;
			score = score / 2 / child->gamesPlayed + sqrt(2 * log(p->gamesPlayed) / child->gamesPlayed);
			if (score >= bestScore)
			{
				bestScore = score;
				bestNode = child;
			}
		}
		p = bestNode;
		bestNode = nullptr;
		bestScore = 0;
	}
	return p;
}
MonteCarloPlayer::TreeNode* MonteCarloPlayer::Expansion(TreeNode* node)
{
	auto availableMoves = node->board.GenerateMoves();
	for (auto move : availableMoves)
	{
		Board newBoard = node->board.MakeMove(move);
		TreeNode* child = new TreeNode(newBoard, node);
		node->children.push_back(child);
	}
	if (node->children.empty())
		return node;
	int idx = rand() % node->children.size();
	return node->children[idx];
}

void MonteCarloPlayer::BackPropagation(TreeNode* node, uint8_t result)
{
	while (node != nullptr)
	{
		node->gamesPlayed++;
		node->totalPoints += result;
		result = 2 - result;
		node = node->parent;
	}
}

void MonteCarloPlayer::InputMove(uint32_t moveMask)
{
	Board newBoard = root->board.MakeMove(moveMask);
	if (root->children.empty())
	{
		auto newRoot = new TreeNode(newBoard, nullptr);
		delete root;
		root = newRoot;
	}
	else
	{
		bool found = false;
		TreeNode* newRoot = nullptr;
		for (auto child : root->children)
		{
			if (!found && child->board.whitePieces == newBoard.whitePieces && child->board.blackPieces == newBoard.blackPieces)
			{
				found = true;
				newRoot = child;
			}
			else
			{
				DeleteNode(child);
			}
		}
		delete root;
		newRoot->parent = nullptr;
		root = newRoot;
		;
	}
}

void MonteCarloPlayer::PerformMC()
{
	uint32_t startTime = clock();
	uint32_t endTime = startTime + timePerMove;
	while (clock() < endTime)
	{
		TreeNode* selectedNode = Selection();
		if (selectedNode->gamesPlayed == 0)
			Expansion(selectedNode);
		uint8_t result = Simulation(selectedNode->board);
		BackPropagation(selectedNode, result);
	}
}
uint32_t MonteCarloPlayer::MakeMove()
{
	PerformMC();
	printf("Games Played: %llu\n", root->gamesPlayed);
	if (root->children.empty()) //root should never be nullptr when player is to move but it can have no children
		return 0;

	double evaluation = (double)root->totalPoints / root->gamesPlayed - 1;
	printf("Evaluation: %.3f (value between -1 and 1, more = better situation)\n", evaluation);

	TreeNode* bestChild = nullptr;
	uint64_t maxSimulations = 0;
	uint32_t bestMove = 0;

	for (auto child : root->children)
	{
		if (child->gamesPlayed > maxSimulations)
		{
			maxSimulations = child->gamesPlayed;
			bestChild = child;
		}
	}
	auto availableMoves = root->board.GenerateMoves();

	for (auto move : availableMoves)
	{
		Board newBoard = root->board.MakeMove(move);
		if (newBoard.whitePieces == bestChild->board.whitePieces && newBoard.blackPieces == bestChild->board.blackPieces)
		{
			bestMove = move;
			break;
		}
	}
	for (auto child : root->children)
	{
		if (child != bestChild)
			DeleteNode(child);
	}
	delete root;
	root = bestChild;
	root->parent = nullptr;

	return bestMove;
}