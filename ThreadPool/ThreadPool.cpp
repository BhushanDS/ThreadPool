#include "threadpool.h"

ThreadPool::ThreadPool(int nThreads) : m_threads(nThreads), stop(false)
{
	for (int i = 0; i < m_threads; i++)
	{	
		threads.emplace_back([this]() {
			std::function<void()>task;
			while (1)
			{
				std::unique_lock<std::mutex>lock(mtx);
				cv.wait(lock, [this] {
					return !tasks.empty() || stop;
				});

				if (stop)
				{
					return;
				}

				task = move(tasks.front());
				tasks.pop();

				lock.unlock();
				task();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex>lock(mtx);
	stop = true;
	lock.unlock();
	cv.notify_all();
	for (auto& thread : threads)
	{
		thread.join();
	}
}

/*
template<class F, class... Args>
auto ThreadPool::ExecuteTask(F&& f, Args&&... args)->std::future<decltype(f(args...))>
{
	using return_type = decltype(f(args...));

	auto task = std::make_shared<std::packaged_task<return_type()>>(bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type>res = task->get_future();

	std::unique_lock<std::mutex>lock(mtx);

	tasks.emplace([task]() -> void {
		(* task)();
		});

	lock.unlock();

	cv.notify_one();

	return res;
}*/

