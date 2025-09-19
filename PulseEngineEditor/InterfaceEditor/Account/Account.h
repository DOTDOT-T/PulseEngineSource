/**
 * @file Account.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief This class define the way account are managed inside the engine. Accounts are Pulse Account you get from signing in Pulse Software.
 * @version 0.1
 * @date 2025-09-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Common/common.h"
#include "Common/dllExport.h"

class PULSE_ENGINE_DLL_API Account
{
    public:
        Account(const std::string& mail, const std::string& password);

        const std::string GetName() const {return name; }
        const std::string GetEmail() const {return email; }
    
    private:
        std::string id;
        std::string name;
        std::string email;
};

#endif