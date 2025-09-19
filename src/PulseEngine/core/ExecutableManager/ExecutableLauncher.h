#ifndef EXECUTABLELauncher_H
#define EXECUTABLELauncher_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/ExecutableManager/ExecutableCommunication.h"

class PULSE_ENGINE_DLL_API ExecutableLauncher
{
    public:
        ExecutableLauncher(const std::string &pathToExe);
        ~ExecutableLauncher();


    private:
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

};

#endif