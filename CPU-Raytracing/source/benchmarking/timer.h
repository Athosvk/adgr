#pragma once
#include <chrono>

namespace CRT
{
	class Timer
	{
	public:
		Timer();

		using Duration = std::chrono::duration<float>;
		Duration GetDuration() const;
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};
}
