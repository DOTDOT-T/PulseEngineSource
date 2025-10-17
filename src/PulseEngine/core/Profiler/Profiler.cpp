
#include "PulseEngine/core/Profiler/Profiler.h"
#include "PulseEngine/core/Profiler/TraceEvent.h"
#include "json.hpp"
#include "Profiler.h"

std::vector<TraceEvent*> Profiler::traceEvents;
std::chrono::steady_clock::time_point Profiler::startTime = std::chrono::steady_clock::now();

Profiler& Profiler::GetInstance()
{
    static Profiler instance;

    return instance;
}

void Profiler::AddTrace(TraceEvent* trace)
{
    traceEvents.push_back(trace);
}

Profiler::~Profiler()
{
    SaveToJson();
}

void Profiler::SaveToJson()
{
    EDITOR_LOG("profiler end")

    std::ofstream outFile("TraceProfiler.json");
    nlohmann::json outJson;

    for (TraceEvent* trace : traceEvents)
    {
        nlohmann::json traceJson;
        traceJson["name"] = trace->name;
        traceJson["ph"] = trace->phase;
        traceJson["ts"] = trace->timeStamp;
        traceJson["tid"] = trace->tid;
        traceJson["pid"] = trace->pid;
        outJson["traceEvents"].push_back(traceJson);
    }

    outFile << outJson.dump(4);

    outFile.close();
}

void Profiler::Clear()
{
    for(TraceEvent* te : traceEvents)
    {
        delete te;
        te = nullptr;
    }
    traceEvents.clear();
}
