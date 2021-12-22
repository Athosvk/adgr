#pragma once
#include "./raytracing/ray.h"

#include <concurrent_queue.h>
#include <thread>
#include <future>
#include <queue>

namespace CRT
{
	class RandomGenerator;

	template<typename TThreadStateType>
	class JobManager
	{
	public:
		using ArgType = TThreadStateType&;
		using JobType = std::function<void(ArgType)>;
		using ThreadInitType = std::function<TThreadStateType(void)>;

		// -1 means use threads equal to the amount of hardware threads
		explicit JobManager(ThreadInitType _threadInit, int _numThreads = -1);

		~JobManager()
		{
			{
				std::unique_lock<std::mutex> lock(m_TaskQueueMutex);
				m_Done = true;
			}
			m_JobReady.notify_all();
			for (std::thread& worker : m_WorkerThreads)
				worker.join();
		}

		template<typename TReturnType>
		auto AddJob(std::function<TReturnType(ArgType&)> job)->std::future<TReturnType>
		{
			using return_type = TReturnType;
			auto task = std::make_shared<std::packaged_task<return_type(ArgType&)>>(job);

			std::future<return_type> future = task->get_future();
			{
				std::unique_lock<std::mutex> lock(m_TaskQueueMutex);

				// don't allow enqueueing after stopping the pool
				if (m_Done)
					throw std::runtime_error("enqueue on stopped ThreadPool");

				m_TaskQueue.emplace([task](ArgType& threadState)
				{
					(*task)(threadState);
				});
			}
			m_JobReady.notify_one();
			return future;
		}
		unsigned GetMaxWorkerThreads()
		{
			auto maxThreads = std::thread::hardware_concurrency();
			if (maxThreads < 2u)
				// Always have one worker thread to distribute jobs to
				return 1u;

			return std::max(1u, maxThreads - 2);
		}
	private:
		std::vector<std::thread> InitThreads(const std::function<TThreadStateType()>& _threadInit, uint32_t _numThreads)
		{
			std::vector<std::thread> threads;
			threads.reserve(_numThreads);
			for (size_t i = 0; i < _numThreads; i++)
			{
				threads.emplace_back([this, _threadInit] {
					TThreadStateType threadState(_threadInit());

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

						task(threadState);
					}
				}
				);
			}
			return threads;
		}

		std::mutex m_TaskQueueMutex;
		std::atomic<bool> m_Done = false;
		std::queue<JobType> m_TaskQueue;
		std::condition_variable m_JobReady;
		std::vector<std::thread> m_WorkerThreads;
	};

	template<typename TThreadStateType>
	JobManager<TThreadStateType>::JobManager(std::function<TThreadStateType()> _threadInit, int _numThreads) :
		m_WorkerThreads(InitThreads(_threadInit, _numThreads == -1 ? GetMaxWorkerThreads() : (uint32_t)_numThreads))
	{
	}
}
