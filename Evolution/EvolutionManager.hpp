#pragma once

#include <vector>
#include <mutex>
#include "Bot.hpp"
#include "EvolutionConfig.hpp"
#include "MonteCarloPlayer.hpp"
#include "MinimaxPlayer.hpp"
#include "Game.hpp"

class EvolutionManager {
public:
    EvolutionManager(bool resume);
    void Run();  

private:
    std::vector<Bot> population;
    Bot previousBest;
    std::mutex scoreMutex;
     
    // Initialization
    void Initialize(bool resume);

    // Tournament
    void RunTournament();
    void PlayMatch(Bot& botA, Bot& botB);
    void PlaySingleMatch(Bot& botA, Bot& botB, bool botAWhite);


    // Evolution
    void Evolve();
    const Bot& SelectParent();
    EvaluationWeights Crossover(const Bot& p1, const Bot& p2);
    EvaluationWeights Mutate(const EvaluationWeights& weights);

    // Evaluation helpers
    int MatchBots(Player* white, Player* black);
    void EvaluateAgainstMonteCarlo(const Bot& bestBot, int generation);
    void SavePopulationToFile(const std::string& filename);
    bool LoadPopulationFromFile(const std::string& filename);

};
