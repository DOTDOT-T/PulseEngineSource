#ifndef TRACEEVENT_H
#define TRACEEVENT_H

#include <string>
#include "common/common.h"
#include "common/dllExport.h"
struct PULSE_ENGINE_DLL_API TraceEvent
{
	std::string name;
	std::string phase;
	int timeStamp;
	int tid;
	int pid;

	TraceEvent(std::string n, std::string p, int ts, int t, int pi)
	{
		name = n;
		phase = p;
		timeStamp = ts;
		tid = t;
		pid = pi;
	}
};

#endif