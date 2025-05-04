#include "MinimaxPlayer.hpp"  
#include "Bot.hpp"            


MinimaxPlayer* Bot::CreatePlayer(bool isWhite, int depth, int timeLimit) const {
    return new MinimaxPlayer(isWhite, timeLimit, weights, depth);
}
