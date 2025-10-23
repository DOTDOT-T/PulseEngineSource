#include <iostream>
#include <random>
#include "PulseEngine/core/GUID/GuidGenerator.h"

#include <string>
#include <functional>
#include <chrono>
#include "Common/common.h"
#include "GuidGenerator.h"


std::size_t GenerateGUIDFromPath(const std::string& filepath)
{
    std::hash<std::string> hasher;
    return hasher(filepath);  // returns same value every time for the same path
}


std::string PULSE_ENGINE_DLL_API GenerateNameOnTime(const std::string& baseName)
{
    using namespace std::chrono;

    auto now = time_point_cast<milliseconds>(steady_clock::now());
    auto ms = now.time_since_epoch().count();

    return baseName + "#" + std::to_string(ms);
}



std::size_t PULSE_ENGINE_DLL_API GenerateGUIDFromPathAndMap(const std::string &filepath, const std::string &mapName)
{
    static std::size_t lastHash = 0;
    std::hash<std::string> hasher;
    std::size_t currentHash = hasher(filepath + "_" + mapName);
    while(lastHash == currentHash)
    {
        currentHash = hasher(filepath + "_" + mapName + std::string("PULSE_ENGINE_SALT"));
    }
    lastHash = currentHash;

    return currentHash;  // returns same value every time for the same path
}
