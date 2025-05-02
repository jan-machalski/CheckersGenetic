#include "MinimaxPlayer.hpp"  
#include "Bot.hpp"            

MinimaxPlayer Bot::CreatePlayer(bool isWhite, int depth, int timeLimit) const {
    return MinimaxPlayer(isWhite, depth, weights, timeLimit);
}
