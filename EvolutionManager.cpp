#include "EvolutionManager.hpp"
#include <algorithm>
#include <future>
#include <iostream>
#include <mutex>
#include <random>


using namespace std;

EvolutionManager::EvolutionManager() {
    Initialize();
}

void EvolutionManager::Run() {
    for (int generation = 0; generation < EvolutionConfig::GENERATIONS; ++generation) {
        cout << "\n=== Generation " << generation + 1 << " ===\n";

        RunTournament();

        // Print scores
        cout << "\n=== Final Bot Scores After Tournament ===\n";
        for (int i = 0; i < population.size(); ++i) {
            cout << "Bot " << i << " Score: " << population[i].score << endl;
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

            cout << "Match Results (New Bot vs Previous Best):\n";
            cout << "  Game 1 (New as Black): " << (result1 == 1 ? "Loss" : result1 == -1 ? "Win" : "Draw") << "\n";
            cout << "  Game 2 (New as White): " << (result2 == 1 ? "Win" : result2 == -1 ? "Loss" : "Draw") << "\n";
            cout << "  Overall Score for New Bot: " << scoreNewBot << " (Win=1, Draw=0, Loss=-1 per game)\n";
        }

        EvaluateAgainstMonteCarlo(bestBot, generation);

        previousBest = bestBot;
        Evolve();

        // Print weights of new generation
        cout << "\n=== New Bot Weights After Evolution ===\n";
        for (int i = 0; i < min(3, (int)population.size()); ++i) {
            cout << "Bot " << i << ": Pieces = " << population[i].weights.piecesWeight
                << ", Kings: " << population[i].weights.kingsWeight << endl;
        }

        cout << "\n=== Bot Details After Evolution ===\n";
        for (int i = 0; i < population.size(); ++i) {
            cout << "Bot " << i << " Weights -> Pieces: "
                << population[i].weights.piecesWeight
                << ", Kings: " << population[i].weights.kingsWeight
                << " | Score: " << population[i].score << endl;
        }
    }
}

void EvolutionManager::Initialize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(EvolutionConfig::MIN_WEIGHT, EvolutionConfig::MAX_WEIGHT);

    for (int i = 0; i < EvolutionConfig::POPULATION_SIZE; ++i) {
        float piecesWeight = dist(gen);
        float kingsWeight = dist(gen);
        population.emplace_back(EvaluationWeights(piecesWeight, kingsWeight));
    }

    cout << "\n=== Initial Bot Weights ===\n";
    for (int i = 0; i < EvolutionConfig::POPULATION_SIZE; ++i) {
        cout << "Bot " << i << ": Pieces = " << population[i].weights.piecesWeight
            << ", Kings = " << population[i].weights.kingsWeight << endl;
    }
}

void EvolutionManager::RunTournament() {
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < population.size(); ++i) {
        for (size_t j = i + 1; j < population.size(); ++j) {
            futures.push_back(std::async(std::launch::async, [this, i, j]() {
                PlayMatch(population[i], population[j]);
                }));
        }
    }

    for (auto& fut : futures) {
        fut.get();
    }
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

    cout << "Gen " << generation + 1 << ": BestBot Wins = " << bestBotWins
        << ", MonteCarlo Wins = " << monteCarloWins
        << ", Draws = " << draws << endl;
}
