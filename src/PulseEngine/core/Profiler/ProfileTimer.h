#ifndef PROFILETIMER_H
#define PROFILETIMER_H
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <iosfwd>
#include <time.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <string>
#include <iostream>
#include "common/common.h"
#include "common/dllExport.h"

class PULSE_ENGINE_DLL_API ProfileTimer
{
public:
    explicit ProfileTimer(const std::string& name = "");

    ~ProfileTimer();

    ProfileTimer(const ProfileTimer&) = delete;
    ProfileTimer& operator=(const ProfileTimer&) = delete;

    ProfileTimer(ProfileTimer&&) = default;
    ProfileTimer& operator=(ProfileTimer&&) = default;

private:
    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
};

#endif // PROFILETIMER_H
