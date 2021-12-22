#pragma once
#include "./raytracing/ray.h"

#include <concurrent_queue.h>
#include <thread>
#include <future>
#include <queue>

namespace CRT
{
	class RandomGenerator;

	class JobManager
	{
	public:
		using JobType = std::function<void(RandomGenerator& generator)>;

		// -1 means use threads equal to the amount of hardware threads
		JobManager(int _numThreads = -1);
		~JobManager();

		template<typename TReturnType>
		auto AddJob(std::function<TReturnType(RandomGenerator&)> job)->std::future<TReturnType>;
		unsigned GetMaxWorkerThreads();
	private:
		std::vector<std::thread> InitThreads(uint32_t _numThreads);

		std::mutex m_TaskQueueMutex;
		std::atomic<bool> m_Done = false;
		std::queue<JobType> m_TaskQueue;
		std::condition_variable m_JobReady;
		std::vector<std::thread> m_WorkerThreads;
	};

	template<typename TReturnType>
	auto JobManager::AddJob(std::function<TReturnType(RandomGenerator& generator)> job) -> 
		std::future<TReturnType>
	{
		using return_type = TReturnType;
		auto task = std::make_shared<std::packaged_task<return_type(RandomGenerator&)>>(job);
			
		std::future<return_type> future = task->get_future();
		{
			std::unique_lock<std::mutex> lock(m_TaskQueueMutex);

			// don't allow enqueueing after stopping the pool
			if(m_Done)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			m_TaskQueue.emplace([task](RandomGenerator& generator)
			{ 
				(*task)(generator); 
			});
		}
		m_JobReady.notify_one();
		return future;
	}
}