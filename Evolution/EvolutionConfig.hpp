#pragma once

namespace EvolutionConfig {
	constexpr int POPULATION_SIZE = 100;
	constexpr int GENERATIONS = 10;


	constexpr float MUTATION_RATE = 0.1f;      
	constexpr float MUTATION_STD_DEV = 0.2f;

	constexpr float MIN_WEIGHT = -1.0f;
	constexpr float MAX_WEIGHT = 1.0f;
	constexpr int TOURNAMENT_ROUNDS = 99;
}
