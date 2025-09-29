#include <iostream>
#include <random>
#include "PulseEngine/core/GUID/GuidGenerator.h"

#include <string>
#include <functional>
#include "Common/common.h"
#include "GuidGenerator.h"


std::size_t GenerateGUIDFromPath(const std::string& filepath)
{
    std::hash<std::string> hasher;
    return hasher(filepath);  // returns same value every time for the same path
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
