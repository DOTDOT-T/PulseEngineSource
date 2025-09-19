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
    std::hash<std::string> hasher;
    return hasher(filepath + "_" + mapName);  // returns same value every time for the same path
}
