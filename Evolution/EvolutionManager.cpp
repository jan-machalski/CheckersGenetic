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

			for (int i = 0; i < static_cast<std::size_t>(Heuristic::COUNT); ++i) {
				outFile << bot.weights.weights[i] << " ";
			}
            outFile << "\n";
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
    float pieces, kings, mobility, singleSupported, doublySupported, score;
    while (inFile >> pieces >> kings >> mobility >> singleSupported >> doublySupported >> score) {
        Bot bot(EvaluationWeights(pieces, kings, mobility, singleSupported, doublySupported));
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
			printf("Evaluating against previous best bot...\n");
            MinimaxPlayer* prevWhite = previousBest.CreatePlayer(true);
            MinimaxPlayer* prevBlack = previousBest.CreatePlayer(false);
            MinimaxPlayer* newWhite = bestBot.CreatePlayer(true);
            MinimaxPlayer* newBlack = bestBot.CreatePlayer(false);

            int result1 = MatchBots(prevWhite, newBlack);
            int result2 = MatchBots(newWhite, prevBlack);
            int scoreNewBot = -result1 + result2;

            delete prevWhite;
            delete prevBlack;
            delete newWhite;
            delete newBlack;

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
            printf("Bot %d: Pieces = %.2f, Kings: %.2f, Mobility: %.2f, Single supported = %.2f, Doubly supported = %.2f\n",
                i, population[i].weights.weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)],
                population[i].weights.weights[static_cast<std::size_t>(Heuristic::KING_COUNT)],
				population[i].weights.weights[static_cast<std::size_t>(Heuristic::MOBILITY)],
                population[i].weights.weights[static_cast<std::size_t>(Heuristic::SINGLE_SUPPORTED)],
                population[i].weights.weights[static_cast<std::size_t>(Heuristic::DOUBLY_SUPPORTED)]
                );
        }
       /* printf("\n=== Bot Details After Evolution ===\n");
        for (int i = 0; i < population.size(); ++i) {
            printf("Bot %d Weights -> Pieces: %.2f, Kings: %.2f | Score: %.2f\n",
                i, population[i].weights.piecesWeight, population[i].weights.kingsWeight, population[i].score);
        }*/

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
        float mobility = dist(gen);
        float singleSupported = dist(gen);
        float doublySupported = dist(gen);
        population.emplace_back(EvaluationWeights(piecesWeight, kingsWeight,mobility,singleSupported,doublySupported));
    }

    printf("\n=== Initial Bot Weights ===\n");
    for (int i = 0; i < EvolutionConfig::POPULATION_SIZE; ++i) {
        printf("Bot %d: Pieces = %.2f, Kings = %.2f, Mobility = %.2f, Single supported = %.2f, Doubly supported = %.2f\n",
            i, population[i].weights.weights[static_cast<std::size_t>(Heuristic::PAWN_COUNT)],
            population[i].weights.weights[static_cast<std::size_t>(Heuristic::KING_COUNT)],
            population[i].weights.weights[static_cast<std::size_t>(Heuristic::MOBILITY)],
            population[i].weights.weights[static_cast<std::size_t>(Heuristic::SINGLE_SUPPORTED)],
            population[i].weights.weights[static_cast<std::size_t>(Heuristic::DOUBLY_SUPPORTED)]);
    }
}

void EvolutionManager::RunTournament() {
    printf("Running tournament\n");

	std::vector<std::future<void>> futures;
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t numBots = population.size();
    std::vector<int> gamesPlayed(numBots, 0);

    int totalGamesNeeded = EvolutionConfig::TOURNAMENT_ROUNDS * numBots / 2;
    int gamesScheduled = 0;

    for (size_t i = 0; i < numBots; ++i) {
        int rival = i;
        while (gamesPlayed[i] < EvolutionConfig::TOURNAMENT_ROUNDS) {
			rival = (rival + 997) % numBots;
            if (gamesPlayed[rival] >= EvolutionConfig::TOURNAMENT_ROUNDS) {
                continue;
            }

            bool iIsWhite = (gamesScheduled % 2 == 0);
            gamesPlayed[i]++;
            gamesPlayed[rival]++;
            gamesScheduled++;

            futures.push_back(std::async(std::launch::async, [this, i, rival, iIsWhite]() {
                PlaySingleMatch(population[i], population[rival], iIsWhite);
                }));
        }
        
    }
	std::atomic<int> completedGames = 0;
    printf("Tournament progress: %d/%d games completed (%.1f%%)         \n",
        completedGames.load(), totalGamesNeeded, 0);
    fflush(stdout);

    for (auto& fut : futures) {
        fut.get();

        completedGames++;
        float progressPercent = (float)completedGames / totalGamesNeeded * 100.0f;

        printf("Tournament progress: %d/%d games completed (%.1f%%)         \r",
            completedGames.load(), totalGamesNeeded, progressPercent);
        fflush(stdout);
    }
    printf("\nTournament completed\n");


}


void EvolutionManager::PlaySingleMatch(Bot& botA, Bot& botB, bool botAWhite) {
    std::unique_ptr<Player> white(botAWhite
        ? botA.CreatePlayer(true)
        : botB.CreatePlayer(true));

    std::unique_ptr<Player> black(botAWhite
        ? botB.CreatePlayer(false)
        : botA.CreatePlayer(false));

    Game game(white.get(), black.get());
    Game::Result result = game.Simulate();

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

    for (int i = 0; i < static_cast<std::size_t>(Heuristic::COUNT); ++i) {
		child.weights[i] = (p1.weights.weights[i] + p2.weights.weights[i]) / 2.0f;
	}
    return child;
}

EvaluationWeights EvolutionManager::Mutate(const EvaluationWeights& w) {
    EvaluationWeights mutated = w;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    std::normal_distribution<float> amount(0.0f, EvolutionConfig::MUTATION_STD_DEV);

	for (int i = 0; i < static_cast<std::size_t>(Heuristic::COUNT); ++i) {
		if (chance(gen) < EvolutionConfig::MUTATION_RATE)
			mutated.weights[i] += amount(gen);
	}
    
	for (int i = 0; i < static_cast<std::size_t>(Heuristic::COUNT); ++i) {
		mutated.weights[i] = std::clamp(mutated.weights[i], EvolutionConfig::MIN_WEIGHT, EvolutionConfig::MAX_WEIGHT);
	}

    return mutated;
}

int EvolutionManager::MatchBots(Player* white, Player* black) {
    Game game(white, black);
    Game::Result result = game.Simulate();
    if (result == Game::WHITE_WINS) return 1;
    else if (result == Game::BLACK_WINS) return -1;
    return 0;
}

void EvolutionManager::EvaluateAgainstMonteCarlo(const Bot& bestBot, int generation) {
    int monteCarloWins = 0;
    int bestBotWins = 0;
    int draws = 0;
    const int gamesPerSide = 5;

	printf("Evaluating against Monte Carlo...\n");
	printf("Games evaluated: 0/%d\n", gamesPerSide);
	fflush(stdout);

    for (int i = 0; i < gamesPerSide; ++i) {
        MonteCarloPlayer* mcWhite = new MonteCarloPlayer(true, 100);
        MonteCarloPlayer* mcBlack = new MonteCarloPlayer(false, 100);
        MinimaxPlayer* botWhite = bestBot.CreatePlayer(true);
        MinimaxPlayer* botBlack = bestBot.CreatePlayer(false);

        int r1 = MatchBots(botWhite, mcBlack);
        int r2 = MatchBots(mcWhite, botBlack);

        if (r1 == 1) bestBotWins++;
        else if (r1 == -1) monteCarloWins++;
        else draws++;

        if (r2 == 1) monteCarloWins++;
        else if (r2 == -1) bestBotWins++;
        else draws++;
		printf("Games evaluated: %d/%d\r", i + 1, gamesPerSide);
        fflush(stdout);

        delete mcWhite;
        delete mcBlack;
        delete botWhite;
        delete botBlack;
    }
    printf("Gen %d: BestBot Wins = %d, MonteCarlo Wins = %d, Draws = %d\n",
        generation + 1, bestBotWins, monteCarloWins, draws);

}
