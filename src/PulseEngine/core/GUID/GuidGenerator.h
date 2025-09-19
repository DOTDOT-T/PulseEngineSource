/**
 * @file GuidGenerator.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GUIDGENERATOR_H
#define GUIDGENERATOR_H

#include "Common/common.h"
#include "Common/dllExport.h"

std::size_t PULSE_ENGINE_DLL_API GenerateGUIDFromPath(const std::string& filepath);
std::size_t PULSE_ENGINE_DLL_API GenerateGUIDFromPathAndMap(const std::string& filepath, const std::string& mapName);

#endif
