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
			printf("Draw!\n");
			return DRAW;
		}

		printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);

		printf("Move %d\n", moveCounter++);
		auto availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = whitePlayer->MakeMove();
		if (newMove == 0)
		{
			printf("White player has no moves left!\n");
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

		printf("White player made move: %s\n", moveMade->notation.c_str());
		printBoard(gameBoard.whitePieces, gameBoard.blackPieces, gameBoard.promotedPieces, true);
		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
			printf("Draw!\n");
			return DRAW;
		}

		availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = blackPlayer->MakeMove();
		if (newMove == 0)
		{
			printf("Black player has no moves left\n");
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

		printf("Black player made move: %s\n", moveMade->notation.c_str());
	}
}

Game::Result Game::Simulate()
{
	Board gameBoard;
	uint32_t newMove = 0;
	int moveCounter = 1;

	while (true)
	{
		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
			return DRAW;
		}

		auto availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = whitePlayer->MakeMove();
		if (newMove == 0)
		{
			return BLACK_WINS;
		}

		auto moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			return BLACK_WINS;
		}
		gameBoard.ApplyMove(newMove);
		blackPlayer->InputMove(newMove);

		if (gameBoard.nonAdvancingMoveCount >= MAX_NON_ADVANCING_MOVE_COUNT)
		{
			return DRAW;
		}

		availableMoves = gameBoard.GenerateMovesWithNotation();

		newMove = blackPlayer->MakeMove();
		if (newMove == 0)
		{
			return WHITE_WINS;
		}
		moveMade = std::find_if(availableMoves.begin(), availableMoves.end(), [newMove](const Move& move) { return move.mask == newMove; });
		if (moveMade == availableMoves.end())
		{
			return WHITE_WINS;
		}

		gameBoard.ApplyMove(newMove);
		whitePlayer->InputMove(newMove);

	}
}


