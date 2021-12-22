#include "random_generator.h"
#include <limits>

namespace CRT
{
	RandomGenerator::RandomGenerator(uint32_t _seed) :
		m_State(_seed)
	{
	}

	float RandomGenerator::NextFloat()
	{
		uint32_t state = m_State;
		state ^= (state << 13);
		state ^= (state << 17);
		state ^= (state << 5);
		constexpr float Multiplier = 1.0f / std::numeric_limits<uint32_t>::max();
		return state * Multiplier;
	}
}