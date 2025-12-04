#include "Gamemode.h"
#include "PulseEngine/core/Gamemode/HudController/HudController.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

PULSE_REGISTER_CLASS_CPP(Gamemode)


void Gamemode::Serialize(Archive& ar) 
{
    hud->Serialize(ar);
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

HudController *Gamemode::GetHudController()
{
    return hud;
}
