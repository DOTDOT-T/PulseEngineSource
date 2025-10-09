#ifndef PROFILER_H
#define PROFILER_H
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <iosfwd>
#include <time.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <vector>
#include "common/dllExport.h"

struct TraceEvent;


class PULSE_ENGINE_DLL_API Profiler
{
public:
	static Profiler& GetInstance();

	void AddTrace(TraceEvent* trace);

	Profiler(const Profiler& p) = delete;
	void operator=(const Profiler& p) = delete;
	static void SaveToJson();
	static void Clear();

	static std::chrono::steady_clock::time_point startTime;

private:
	Profiler() { }
	~Profiler();

	static std::vector<TraceEvent*> traceEvents;
};


#endif