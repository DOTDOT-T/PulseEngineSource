#ifndef IMODULE_H
#define IMODULE_H

// #include "Common/common.h"
#include "Common/dllExport.h"
#include <string>

class PULSE_ENGINE_DLL_API IModule
{
    public:
        /**
         * @brief Get the name of the module.
         * @return std::string The name of the module.
         */
        virtual std::string GetName() const = 0;

        /**
         * @brief Get the version of the module.
         * @return std::string The version of the module.
         */
        virtual std::string GetVersion() const = 0;

        /**
         * @brief Initialize the module.
         */
        virtual void Initialize() = 0;

        /**
         * @brief Shutdown the module.
         */
        virtual void Shutdown() = 0;

        virtual ~IModule() = default;
};

#endif