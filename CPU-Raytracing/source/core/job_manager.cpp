#include "job_manager.h"

#include <./core/random_generator.h>
#include <random>

namespace CRT
{
	JobManager::JobManager(int _numThreads) :
		m_WorkerThreads(InitThreads(_numThreads == -1 ? GetMaxWorkerThreads() : (uint32_t)_numThreads))
	{
	}

	JobManager::~JobManager()
	{
		{
			std::unique_lock<std::mutex> lock(m_TaskQueueMutex);
			m_Done = true;
		}
		m_JobReady.notify_all();
		for(std::thread& worker : m_WorkerThreads)
			worker.join();
	}

	std::vector<std::thread> CRT::JobManager::InitThreads(uint32_t _numThreads)
	{
		std::vector<std::thread> threads;
		threads.reserve(_numThreads);
		for (size_t i = 0; i < _numThreads; i++)
		{
			threads.emplace_back([this] {
				RandomGenerator generator{ std::random_device()() };

				while (true)
				{
					JobType task;
					{
						std::unique_lock<std::mutex> lock(m_TaskQueueMutex);
						m_JobReady.wait(lock,
							[this] { return m_Done || !m_TaskQueue.empty(); });
						if (m_Done && m_TaskQueue.empty())
							return;
						task = std::move(m_TaskQueue.front());
						m_TaskQueue.pop();
					}

					task(generator);
				}
			}
			);
		}
		return threads;
	}

	unsigned JobManager::GetMaxWorkerThreads()
	{
		auto maxThreads = std::thread::hardware_concurrency();
		if (maxThreads < 2u)
			// Always have one worker thread to distribute jobs to
			return 1u;

		return std::max(1u, maxThreads - 2);
	}
}
