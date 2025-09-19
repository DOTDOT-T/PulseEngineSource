#include "PulseExecutable.h"

PulseExecutable::PulseExecutable(const std::string &exePath, const std::string &pipeName)
{
    this->exePath = exePath;
    this->pipeName = pipeName;
    launcher = new ExecutableLauncher(exePath);

    Sleep(1500); // Give some time for the executable to start

    if (pipeName != "NONE") comm = new ExecutableCommunication(pipeName);
    else                    comm = nullptr;
}

PulseExecutable::~PulseExecutable()
{
    Terminate();
    delete launcher;
    delete comm;
}

void PulseExecutable::Terminate()
{
    EDITOR_LOG("Terminating executable: " + exePath);
    if(launcher) delete launcher;
    if(comm) {
        EDITOR_LOG("Terminating communication pipe: " + pipeName);
        comm->TerminateCommunication();
        delete comm;
    }
    launcher = nullptr;
    comm = nullptr;
}

void PulseExecutable::SendExeMessage(const std::string &message)
{
    if(comm) comm->SendMessageToExecutable(message);
}

std::string PulseExecutable::ReceiveMessage()
{
    if(comm) return comm->ReadMessageFromExecutable();
    else return "No pipe communication available.";
}
