#ifndef PULSE_EXECUTABLE_H
#define PULSE_EXECUTABLE_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/ExecutableManager/ExecutableLauncher.h"
#include "PulseEngine/core/ExecutableManager/ExecutableCommunication.h"

class PULSE_ENGINE_DLL_API PulseExecutable
{
public:
    PulseExecutable(const std::string& exePath, const std::string& pipeName = "NONE");
    ~PulseExecutable();

    void Terminate();

    void SendExeMessage(const std::string& message);
    std::string ReceiveMessage();

private:
    std::string exePath;
    std::string pipeName;

    // Composition: PulseExecutable owns these subsystems
    ExecutableLauncher* launcher = nullptr;
    ExecutableCommunication* comm = nullptr;
};

#endif
