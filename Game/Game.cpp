#include "Game.hpp"
#include "Utils.hpp"

Game::Result Game::Play()
{
	Board gameBoard;
	uint32_t newMove = 0;
	int moveCounter = 1;

	while (true)
	{
		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
		//	printf("Draw!\n");
			return DRAW;
		}

		//printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);

	//	printf("Move %d\n", moveCounter++);
		auto availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = whitePlayer->MakeMove();
		if (newMove == 0)
		{
		//	printf("White player has no moves left!\n");
			return BLACK_WINS;
		}

		auto moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			printf("Invalid move by white player!\n");
			return BLACK_WINS;
		}
		gameBoard.ApplyMove(newMove);
		blackPlayer->InputMove(newMove);

	//	printf("White player made move: %s\n", moveMade->notation.c_str());
		//printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);
		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
		//	printf("Draw!\n");
			return DRAW;
		}

		availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = blackPlayer->MakeMove();
		if (newMove == 0)
		{
		//	printf("Black player has no moves left\n");
			return WHITE_WINS;
		}
		moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			printf("Invalid move made by black player!\n");
			return WHITE_WINS;
		}

		gameBoard.ApplyMove(newMove);
		whitePlayer->InputMove(newMove);

	//	printf("Black player made move: %s\n", moveMade->notation.c_str());
	}
}


std::future<std::pair<Game::Result, std::vector<std::string>>> Game::SimulateAsync()
{
	return std::async(std::launch::async, [this]() {
		Board gameBoard;
		uint32_t newMove = 0;
		int moveCounter = 1;
		std::vector<std::string> moveHistory;

		while (true)
		{
			if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
			{
				return std::make_pair(DRAW, moveHistory);
			}

			//Simulate white player's move
			auto availableMoves = gameBoard.GenerateMovesWithNotation();

			newMove = whitePlayer->MakeMove();
			if (newMove == 0)
			{
				return std::make_pair(BLACK_WINS, moveHistory);
			}

			auto moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
			if (moveMade == availableMoves.end())
			{
				return std::make_pair(BLACK_WINS, moveHistory);
			}
			moveHistory.push_back(moveMade->notation);
			blackPlayer->InputMove(newMove);
			gameBoard.ApplyMove(newMove);

			if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
			{
				return std::make_pair(DRAW, moveHistory);
			}
			// Simulate black player's move

			availableMoves = gameBoard.GenerateMovesWithNotation();

			newMove = blackPlayer->MakeMove();
			if (newMove == 0)
			{
				return std::make_pair(WHITE_WINS, moveHistory);
			}
			moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
			if (moveMade == availableMoves.end())
			{
				return std::make_pair(WHITE_WINS, moveHistory);
			}
			moveHistory.push_back(moveMade->notation);
			whitePlayer->InputMove(newMove);
			gameBoard.ApplyMove(newMove);
		}
	});
}		