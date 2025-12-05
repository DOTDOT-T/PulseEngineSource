/**
 * @file UiHelperTool.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief helper class with static class to easily create UI-engine-style design.
 * @version 0.1
 * @date 2025-12-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __UIHELPERTOOL_H__
#define __UIHELPERTOOL_H__

#include "Common/common.h"
#include "Common/dllExport.h"

#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"

class IScript;

class PULSE_ENGINE_DLL_API UiHelperTool
{
public:
    static void DisplayScriptEditor(IScript* script, int scriptIndex);
    static ContextMenuItem GenerateContextHeader(const char* headerName);
private:
};

#endif // __UIHELPERTOOL_H__