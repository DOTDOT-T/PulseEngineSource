#ifndef __GAMEMODE_H__
#define __GAMEMODE_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

class HudController;

class PULSE_ENGINE_DLL_API Gamemode : public PulseObject
{
PULSE_GEN_BODY(Gamemode)
PULSE_REGISTER_CLASS_HEADER(Gamemode)
public:
    Gamemode();

    void Update();
    void Render();

private:
    HudController* hud = nullptr;
};


#endif // __GAMEMODE_H__