#include "MonteCarloPlayer.hpp"
#include "HumanPlayer.hpp"

#include <future>
#include <string>

class Game
{
	Player* whitePlayer;
	Player* blackPlayer;
public:
	Game(Player* whitePlayer, Player* blackPlayer)
		: whitePlayer(whitePlayer), blackPlayer(blackPlayer)
	{
	}

	enum Result
	{
		WHITE_WINS,
		BLACK_WINS,
		DRAW,
	};

	void Play();
	std::future<std::pair<Result, std::vector<std::string>>> SimulateAsync();

};