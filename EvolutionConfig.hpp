#pragma once

namespace EvolutionConfig {
	constexpr int POPULATION_SIZE = 10;
	constexpr int GENERATIONS = 6;


	constexpr float MUTATION_RATE = 0.1f;       // 10% chance to mutate each weight
	constexpr float MUTATION_STD_DEV = 0.2f;
	constexpr float INITIAL_SIGMA = 0.5f;
	constexpr float SIGMA_DECAY = 0.95f;

	constexpr float MIN_WEIGHT = -1.0f;
	constexpr float MAX_WEIGHT = 1.0f;
}
