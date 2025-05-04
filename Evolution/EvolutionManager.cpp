#include "EvolutionManager.hpp"
#include <algorithm>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fstream>
#include <atomic>

using namespace std;

EvolutionManager::EvolutionManager(bool resume) {
    Initialize(resume);
}


std::atomic<bool> stopEvolution = false;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        stopEvolution = true;
        return TRUE;
    }
    return FALSE;
}

void EvolutionManager::SavePopulationToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& bot : population) {
            outFile << bot.weights.piecesWeight << " "
                << bot.weights.kingsWeight << " "
                << bot.score << "\n";
        }
        outFile.close();
        printf("Progress saved to '%s'\n", filename.c_str());
    }
    else {
        printf("Failed to save progress.\n");
    }
}

bool EvolutionManager::LoadPopulationFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    population.clear();
    float pieces, kings, score;
    while (inFile >> pieces >> kings >> score) {
        Bot bot(EvaluationWeights(pieces, kings));
        bot.score = score;
        population.push_back(bot);
    }

    inFile.close();
    printf("Population loaded from '%s'\n", filename.c_str());
    return !population.empty();
}


void EvolutionManager::Run() {
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);  // Handle Ctrl+C on Windows

    for (int generation = 0; generation < EvolutionConfig::GENERATIONS; ++generation) {
        if (stopEvolution) break;

        printf("\n=== Generation %d ===\n", generation + 1);

        RunTournament();

        printf("\n=== Final Bot Scores After Tournament ===\n");
        for (int i = 0; i < population.size(); ++i) {
            printf("Bot %d Score: %.2f\n", i, population[i].score);
        }

        if (stopEvolution) {
            SavePopulationToFile("evolution_save.txt");
            printf("Evolution interrupted. Progress saved.\n");
            break;
        }

        Bot bestBot = *std::max_element(population.begin(), population.end(),
            [](const Bot& a, const Bot& b) { return a.score < b.score; });

        if (generation > 0) {
            MinimaxPlayer prevWhite = previousBest.CreatePlayer(true);
            MinimaxPlayer prevBlack = previousBest.CreatePlayer(false);
            MinimaxPlayer newWhite = bestBot.CreatePlayer(true);
            MinimaxPlayer newBlack = bestBot.CreatePlayer(false);

            int result1 = MatchBots(&prevWhite, &newBlack);
            int result2 = MatchBots(&newWhite, &prevBlack);
            int scoreNewBot = -result1 + result2;

            printf("Match Results (New Bot vs Previous Best):\n");
            printf("  Game 1 (New as Black): %s\n",
                (result1 == 1 ? "Loss" : result1 == -1 ? "Win" : "Draw"));
            printf("  Game 2 (New as White): %s\n",
                (result2 == 1 ? "Win" : result2 == -1 ? "Loss" : "Draw"));
            printf("  Overall Score for New Bot: %d\n", scoreNewBot);
        }

        EvaluateAgainstMonteCarlo(bestBot, generation);
        previousBest = bestBot;
        Evolve();

        printf("\n=== New Bot Weights After Evolution ===\n");
        for (int i = 0; i < (int)population.size(); ++i) {
            printf("Bot %d: Pieces = %.2f, Kings: %.2f\n",
                i, population[i].weights.piecesWeight, population[i].weights.kingsWeight);
        }
        printf("\n=== Bot Details After Evolution ===\n");
        for (int i = 0; i < population.size(); ++i) {
            printf("Bot %d Weights -> Pieces: %.2f, Kings: %.2f | Score: %.2f\n",
                i, population[i].weights.piecesWeight, population[i].weights.kingsWeight, population[i].score);
        }

        if (stopEvolution) {
            SavePopulationToFile("evolution_save.txt");
            printf("Evolution interrupted. Progress saved.\n");
            break;
        }
    }
}

void EvolutionManager::Initialize(bool resume) {
    population.clear();

    if (resume) {
        if (LoadPopulationFromFile("evolution_save.txt")) {
            printf("Population loaded from file. Resuming...\n");
            return;
        }
        else {
            printf("No saved file found. Starting fresh.\n");
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(EvolutionConfig::MIN_WEIGHT, EvolutionConfig::MAX_WEIGHT);

    for (int i = 0; i < EvolutionConfig::POPULATION_SIZE; ++i) {
        float piecesWeight = dist(gen);
        float kingsWeight = dist(gen);
        population.emplace_back(EvaluationWeights(piecesWeight, kingsWeight));
    }

    printf("\n=== Initial Bot Weights ===\n");
    for (int i = 0; i < EvolutionConfig::POPULATION_SIZE; ++i) {
        printf("Bot %d: Pieces = %.2f, Kings = %.2f\n",
            i, population[i].weights.piecesWeight, population[i].weights.kingsWeight);
    }
}

void EvolutionManager::RunTournament() {
    std::vector<std::future<void>> futures;
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t numBots = population.size();
    std::vector<int> gamesPlayed(numBots, 0);

    int totalGamesNeeded = EvolutionConfig::TOURNAMENT_ROUNDS * numBots / 2;
    int gamesScheduled = 0;

    while (gamesScheduled < totalGamesNeeded) {
        size_t i = gen() % numBots;
        size_t j;
        do {
            j = gen() % numBots;
        } while (i == j);

        if (gamesPlayed[i] >= EvolutionConfig::TOURNAMENT_ROUNDS ||
            gamesPlayed[j] >= EvolutionConfig::TOURNAMENT_ROUNDS) {
            continue;
        }

        bool iIsWhite = gen() % 2 == 0;
        gamesPlayed[i]++;
        gamesPlayed[j]++;
        gamesScheduled++;

        futures.push_back(std::async(std::launch::async, [this, i, j, iIsWhite]() {
            PlaySingleMatch(population[i], population[j], iIsWhite);
            }));
    }

    for (auto& fut : futures) {
        fut.get();
    }
}


void EvolutionManager::PlaySingleMatch(Bot& botA, Bot& botB, bool botAWhite) {
    Player* white = botAWhite ? new MinimaxPlayer(botA.CreatePlayer(true)) : new MinimaxPlayer(botB.CreatePlayer(true));
    Player* black = botAWhite ? new MinimaxPlayer(botB.CreatePlayer(false)) : new MinimaxPlayer(botA.CreatePlayer(false));

    Game game(white, black);
    Game::Result result = game.Play();

    std::lock_guard<std::mutex> lock(scoreMutex);
    if (result == Game::WHITE_WINS) {
        (botAWhite ? botA : botB).score += 2;
    }
    else if (result == Game::BLACK_WINS) {
        (botAWhite ? botB : botA).score += 2;
    }
    else {
        botA.score += 1;
        botB.score += 1;
    }

    delete white;
    delete black;
}



void EvolutionManager::PlayMatch(Bot& botA, Bot& botB) {
    auto whiteA = botA.CreatePlayer(true);
    auto blackB = botB.CreatePlayer(false);
    Game game1(&whiteA, &blackB);
    Game::Result result1 = game1.Play();

    {
        std::lock_guard<std::mutex> lock(scoreMutex);
        if (result1 == Game::WHITE_WINS) botA.score += 2;
        else if (result1 == Game::BLACK_WINS) botB.score += 2;
        else { botA.score += 1; botB.score += 1; }
    }

    auto whiteB = botB.CreatePlayer(true);
    auto blackA = botA.CreatePlayer(false);
    Game game2(&whiteB, &blackA);
    Game::Result result2 = game2.Play();

    {
        std::lock_guard<std::mutex> lock(scoreMutex);
        if (result2 == Game::WHITE_WINS) botB.score += 2;
        else if (result2 == Game::BLACK_WINS) botA.score += 2;
        else { botA.score += 1; botB.score += 1; }
    }
}

void EvolutionManager::Evolve() {
    std::vector<Bot> newPopulation;
    auto bestBot = *std::max_element(population.begin(), population.end(),
        [](const Bot& a, const Bot& b) { return a.score < b.score; });
    bestBot.score = 0;
    newPopulation.push_back(bestBot);

    while (newPopulation.size() < population.size()) {
        const Bot& parent1 = SelectParent();
        const Bot& parent2 = SelectParent();
        EvaluationWeights childWeights = Crossover(parent1, parent2);
        childWeights = Mutate(childWeights);
        newPopulation.emplace_back(childWeights);
    }

    population = newPopulation;
}

const Bot& EvolutionManager::SelectParent() {
    float totalScore = 0.0f;
    for (const auto& bot : population) {
        totalScore += bot.score;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, totalScore);

    float pick = dist(gen);
    float runningSum = 0.0f;

    for (const auto& bot : population) {
        runningSum += bot.score;
        if (runningSum >= pick) {
            return bot;
        }
    }
    return population.back();
}

EvaluationWeights EvolutionManager::Crossover(const Bot& p1, const Bot& p2) {
    EvaluationWeights child;
    child.piecesWeight = (p1.weights.piecesWeight + p2.weights.piecesWeight) / 2.0f;
    child.kingsWeight = (p1.weights.kingsWeight + p2.weights.kingsWeight) / 2.0f;
    return child;
}

EvaluationWeights EvolutionManager::Mutate(const EvaluationWeights& w) {
    EvaluationWeights mutated = w;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    std::normal_distribution<float> amount(0.0f, EvolutionConfig::MUTATION_STD_DEV);

    if (chance(gen) < EvolutionConfig::MUTATION_RATE)
        mutated.piecesWeight += amount(gen);
    if (chance(gen) < EvolutionConfig::MUTATION_RATE)
        mutated.kingsWeight += amount(gen);

    mutated.piecesWeight = std::clamp(mutated.piecesWeight, EvolutionConfig::MIN_WEIGHT, EvolutionConfig::MAX_WEIGHT);
    mutated.kingsWeight = std::clamp(mutated.kingsWeight, EvolutionConfig::MIN_WEIGHT, EvolutionConfig::MAX_WEIGHT);

    return mutated;
}

int EvolutionManager::MatchBots(Player* white, Player* black) {
    Game game(white, black);
    Game::Result result = game.Play();
    if (result == Game::WHITE_WINS) return 1;
    else if (result == Game::BLACK_WINS) return -1;
    return 0;
}

void EvolutionManager::EvaluateAgainstMonteCarlo(const Bot& bestBot, int generation) {
    int monteCarloWins = 0;
    int bestBotWins = 0;
    int draws = 0;
    const int gamesPerSide = 5;

    for (int i = 0; i < gamesPerSide; ++i) {
        MonteCarloPlayer mcWhite(true, 100);
        MonteCarloPlayer mcBlack(false, 100);
        MinimaxPlayer botWhite = bestBot.CreatePlayer(true);
        MinimaxPlayer botBlack = bestBot.CreatePlayer(false);

        int r1 = MatchBots(&botWhite, &mcBlack);
        int r2 = MatchBots(&mcWhite, &botBlack);

        if (r1 == 1) bestBotWins++;
        else if (r1 == -1) monteCarloWins++;
        else draws++;

        if (r2 == 1) monteCarloWins++;
        else if (r2 == -1) bestBotWins++;
        else draws++;
    }
    printf("Gen %d: BestBot Wins = %d, MonteCarlo Wins = %d, Draws = %d\n",
        generation + 1, bestBotWins, monteCarloWins, draws);

}
