#include "Timer.h"

namespace CRT
{
	Timer::Timer()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	Timer::Duration Timer::GetDuration() const
	{
		auto ret = std::chrono::high_resolution_clock::now() - m_StartTime;
		return ret;
	}
}
