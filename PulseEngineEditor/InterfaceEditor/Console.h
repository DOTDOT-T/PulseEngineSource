#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "pch.h"

#include "Common/dllExport.h"


#include <vector>
#include <string>

class PULSE_ENGINE_DLL_API Console
{
public:

    void Render();

    static std::vector<std::string>& GetMessages()
    {
        static std::vector<std::string> messages_safe;
        return messages_safe;
    }

    static void Push(const std::string& msg)
    {
        GetMessages().push_back(msg);
    }
};

#endif // __CONSOLE_H__