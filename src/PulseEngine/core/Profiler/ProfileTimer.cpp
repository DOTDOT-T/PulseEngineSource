
#include "PulseEngine/core/Profiler/Profiler.h"
#include "PulseEngine/core/Profiler/TraceEvent.h"
#include "PulseEngine/core/Profiler/ProfileTimer.h"
#include <chrono>

#include <processthreadsapi.h>

ProfileTimer::ProfileTimer(const std::string& name)
    : m_name(name),
    m_start(std::chrono::steady_clock::now())
{
    using namespace std::chrono;
    const auto startUs = duration_cast<microseconds>(m_start - Profiler::startTime).count();

    TraceEvent* traceStart = new TraceEvent(
        m_name,
        "B",
        startUs,
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );

    Profiler::GetInstance().AddTrace(traceStart);
}

ProfileTimer::~ProfileTimer()
{
    using namespace std::chrono;
    const auto end = steady_clock::now();
    const auto endUs = duration_cast<microseconds>(end - Profiler::startTime).count();

    TraceEvent* traceEnd = new TraceEvent(
        m_name,
        "E",
        endUs,
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );

    Profiler::GetInstance().AddTrace(traceEnd);
}
