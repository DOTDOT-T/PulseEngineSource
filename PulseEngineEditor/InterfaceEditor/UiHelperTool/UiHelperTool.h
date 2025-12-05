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