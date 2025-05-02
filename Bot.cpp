#include "MinimaxPlayer.hpp"  
#include "Evolution/Bot.hpp"            


MinimaxPlayer Bot::CreatePlayer(bool isWhite, int depth, int timeLimit) const {
    return MinimaxPlayer(isWhite, depth, weights, timeLimit);
}
