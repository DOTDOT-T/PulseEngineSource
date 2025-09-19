/**
 * @file ScriptsLoader.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief The script loader loads the dll that contains the user scripts and provides a way to access them.
 * @version 0.1
 * @date 2025-07-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SCRIPTSLOADER_H
#define SCRIPTSLOADER_H

#include "Common/common.h"
#include "Common/dllExport.h"

#include <map>
#include <string>


class IScript;
typedef IScript* (*CreateScriptFunc)();

class PULSE_ENGINE_DLL_API ScriptsLoader
{
    public:
        static HMODULE customScriptDll;
        static HMODULE pulseCustomScriptDll;
        static std::map<std::string, CreateScriptFunc> scriptMap;
        /**
         * @brief Get the Script From Call Name object
         * @brief This function will retrieve the script from the dll and create the IScript*.
         * @brief If no dll loaded, it will try to load it. No crash if the dll is not loaded. It will just return a nullptr.
         * 
         * @param callName Name of the function to call in the dll.
         * @note The function should be exported from the dll with the name "PulseScript"
         * @return IScript* the actual script. it will be a child that inherit from this interface but its not important, it has the Update and Start functions.
         */
        static IScript* GetScriptFromCallName(std::string callName);
        /**
         * @brief Load the specific dll of custom scripts (CustomScripts.dll) and load the exported functions.
         * @brief each function should start with "PulseScript" to specify that it is a script function.
         * @brief this function will give the possibility to GetScriptFromCallName to retrieve the script from the dll and create the IScript*
         * 
         */
        static void LoadDLL();
        /**
         * @brief A simple function to be called before a new dll generation.
         * @brief the customScript.dll will be locked by the system, so we need to free it before generating a new one, or modifying it.
         * 
         */
        static void FreeDll();
        static void LoadExportedFunctions(HMODULE dll);

};


#endif