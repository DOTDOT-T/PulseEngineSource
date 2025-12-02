#include "Gamemode.h"
#include "PulseEngine/core/Gamemode/HudController/HudController.h"

PULSE_REGISTER_CLASS_CPP(Gamemode)


void Gamemode::Serialize(Archive& ar) 
{

}
void Gamemode::Deserialize(Archive& ar) 
{

}
const char* Gamemode::ToString() 
{
    return "Gamemode";   
}

Gamemode::Gamemode()
{
    hud = new HudController;
    hud->Init();
}

void Gamemode::Update()
{
    hud->Update();
}

void Gamemode::Render()
{
    hud->Render();
}
