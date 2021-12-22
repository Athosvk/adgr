#pragma once
#include <cstdint>

namespace CRT
{
	class RandomGenerator
	{
	public:
		RandomGenerator(uint32_t _seed);
		float NextFloat();

	private:
		uint32_t m_State;
	};
}
