#include "JobSystem.h"

JobSystem::JobSystem(size_t workerCount)
    : m_shutdown(false)
{
    m_workers.reserve(workerCount);
    for (size_t i = 0; i < workerCount; ++i)
    {
        m_workers.emplace_back(
            [this]
            {
                WorkerLoop();
            });
    }
}

JobSystem::~JobSystem()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_shutdown = true;
    }
    m_cv.notify_all();
    for (auto &t : m_workers)
    {
        if (t.joinable())
            t.join();
    }
}

void JobSystem::WorkerLoop()
{
    while (true)
    {
        Job job;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]
                      { return m_shutdown || !m_jobs.empty(); });
            if (m_shutdown && m_jobs.empty())
                return;
            job = std::move(m_jobs.front());
            m_jobs.pop();
        }
        job();
    }
}

JobHandle JobSystem::Submit(Job fn)
{
    JobHandle handle;
    auto counter = handle.counter;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_jobs.emplace([fn, counter]
                       {
                fn();
                counter->fetch_sub(1, std::memory_order_release); });
    }
    m_cv.notify_one();
    return handle;
}
