/**
 * @file JobSystem.h
 * @brief Lightweight job system with worker threads and atomic synchronization.
 * @version 0.1
 * @date 2025-11-30
 */
#ifndef __JOBSYSTEM_H__
#define __JOBSYSTEM_H__

#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>

using Job = std::function<void()>;

struct JobHandle
{
    std::shared_ptr<std::atomic<int>> counter;
    JobHandle() : counter(std::make_shared<std::atomic<int>>(1)) {}
    void Wait() const
    {
        while (counter->load(std::memory_order_acquire) > 0)
        {
            std::this_thread::yield();
        }
    }
};

class JobSystem
{
public:

    JobSystem(size_t workerCount = std::thread::hardware_concurrency());

    ~JobSystem();

    JobHandle Submit(Job fn);

private:
    void WorkerLoop();

    std::vector<std::thread> m_workers;
    std::queue<Job> m_jobs;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_shutdown;
};

#endif // __JOBSYSTEM_H__