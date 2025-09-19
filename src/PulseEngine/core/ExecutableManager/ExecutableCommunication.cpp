#include "ExecutableCommunication.h"

ExecutableCommunication::ExecutableCommunication(const std::string &name)
{
    pipeName = R"(\\.\pipe\)" + name;

    while (true) {
        hPipe = CreateFileA(pipeName.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        Sleep(100); // wait for the pipe to be ready
    }
}

ExecutableCommunication::~ExecutableCommunication()
{    
    TerminateCommunication();
}

void ExecutableCommunication::TerminateCommunication()
{
    if (hPipe != INVALID_HANDLE_VALUE) {
        const char* msg = "quit";
        DWORD written;
        WriteFile(hPipe, msg, strlen(msg), &written, nullptr);
        FlushFileBuffers(hPipe);
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

void ExecutableCommunication::SendMessageToExecutable(const std::string &message)
{
    DWORD bytesWritten;
    WriteFile(hPipe, message.c_str(), message.size(), &bytesWritten, nullptr);
}

std::string ExecutableCommunication::ReadMessageFromExecutable()
{
    //not implemented yet
    return std::string("not implemented yet");
}
