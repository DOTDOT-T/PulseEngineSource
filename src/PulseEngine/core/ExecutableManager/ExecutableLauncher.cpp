#include "ExecutableLauncher.h"

ExecutableLauncher::ExecutableLauncher(const std::string &pathToExe)
{
#ifdef PULSE_WINDOWS
    si.cb = sizeof(si);

    // Prevent a new window from showing
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (!CreateProcessA(
        pathToExe.c_str(),   // path to executable
        nullptr,             // command line arguments
        nullptr,             // process security
        nullptr,             // thread security
        FALSE,               // inherit handles
        CREATE_NO_WINDOW,    // creation flags
        nullptr,             // environment
        nullptr,             // current directory
        &si,                 // startup info
        &pi                  // process info
    )) {
        EDITOR_ERROR("Failed to launch another executable: " + std::to_string(GetLastError()));
        return;
    }

    // Close handles (process will continue running)
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    EDITOR_LOG("Launched executable: " + pathToExe);
    return;
#endif
}

ExecutableLauncher::~ExecutableLauncher()
{
}
