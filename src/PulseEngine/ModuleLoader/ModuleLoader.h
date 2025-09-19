/**
 * @file ModuleLoader.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include <map>
#include <string>

class IModule;
typedef IModule* (*CreateModuleFunc)();

class PULSE_ENGINE_DLL_API ModuleLoader
{
    public:
        static IModule* GetModuleFromPath(const std::string& modulePath);
};


#endif