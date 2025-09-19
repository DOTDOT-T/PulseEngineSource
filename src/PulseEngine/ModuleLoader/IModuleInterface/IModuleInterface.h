#ifndef IMODULEINTERFACE_H
#define IMODULEINTERFACE_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/ModuleLoader/IModule/IModule.h"

class PULSE_ENGINE_DLL_API IModuleInterface : public IModule
{
    public:
        virtual void Render() = 0;

        virtual ~IModuleInterface() = default;
};


#endif