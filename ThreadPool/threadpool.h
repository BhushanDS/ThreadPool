#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <utility>
#include <memory>

class ThreadPool
{
private:
	bool stop;
	int m_threads;
	std::mutex mtx;
	std::condition_variable cv;
	std::vector<std::thread>threads;
	std::queue<std::function<void()>>tasks;
public:
	ThreadPool(int nThreads);

	~ThreadPool();

	template<class F, class... Args>
	auto ExecuteTask(F&& f, Args&&... args) -> std::future<decltype(f(args ...))>
	{
		using return_type = decltype(f(args...));

		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type>res = task->get_future();

		std::unique_lock<std::mutex>lock(mtx);

		tasks.emplace([task]() -> void {
			(*task)();
			});

		lock.unlock();

		cv.notify_one();

		return res;
	};
};
