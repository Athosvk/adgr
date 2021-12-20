#pragma once
#include <cstdint>
#include <array>

namespace CRT
{
	template<typename Type, uint32_t NumSamples>
	class RollingSampler
	{
	public:
		void AddSample(Type sample)
		{
			m_Samples[m_CurrentIndex] = sample;
			m_CurrentIndex = (m_CurrentIndex + 1) % NumSamples;
			if (m_SampleCount < NumSamples)
			{
				m_SampleCount++;
			}
		}

		Type GetAverage() const
		{
			Type average = Type(0.f);
			for (size_t i = 0; i < m_SampleCount; i++)
			{
				average += m_Samples[i];
			}
			return average / m_SampleCount;
		}

	private:
		uint32_t m_CurrentIndex = 0;
		uint32_t m_SampleCount = 0;
		std::array<Type, NumSamples> m_Samples;
	};
}
