#include "./raytracing/ray.h"

#include <concurrent_queue.h>
#include <thread>
#include <future>
#include <queue>

namespace CRT
{
	class JobManager
	{
	public:
		// -1 means use threads equal to the amount of hardware threads
		JobManager(int _numThreads = -1);
		~JobManager();

		//< It's only safe to call this from a single thread (i.e. the thread that created the JobManager)
		template<class F, class... Args>
		auto AddJob(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>;
		unsigned GetMaxWorkerThreads();
	private:
		std::vector<std::thread> InitThreads(uint32_t _numThreads);

		std::mutex m_TaskQueueMutex;
		std::atomic<bool> m_Done = false;
		std::queue<std::function<void()>> m_TaskQueue;
		std::condition_variable m_JobReady;
		std::vector<std::thread> m_WorkerThreads;
	};

	// add new work item to the pool
	template<class F, class... Args>
	auto JobManager::AddJob(F&& f, Args&&... args) 
		-> std::future<typename std::invoke_result<F, Args...>::type>
	{
		using return_type = typename std::invoke_result<F, Args...>::type;
		auto task = std::make_shared< std::packaged_task<return_type()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);
			
		std::future<return_type> future = task->get_future();
		{
			std::unique_lock<std::mutex> lock(m_TaskQueueMutex);

			// don't allow enqueueing after stopping the pool
			if(m_Done)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			m_TaskQueue.emplace([task](){ (*task)(); });
		}
		m_JobReady.notify_one();
		return future;
	}
}