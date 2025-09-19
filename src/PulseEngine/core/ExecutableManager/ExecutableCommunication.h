#ifndef EXECUTABLECOMMUNICATION_H
#define EXECUTABLECOMMUNICATION_H

#include "Common/common.h"
#include "Common/dllExport.h"

class PULSE_ENGINE_DLL_API ExecutableCommunication
{
    public:
        ExecutableCommunication(const std::string& name);
        ~ExecutableCommunication();

        void TerminateCommunication();

        void SendMessageToExecutable(const std::string &message);
        std::string ReadMessageFromExecutable();

    private:
        HANDLE hPipe = INVALID_HANDLE_VALUE;
        std::string pipeName;
};


#endif