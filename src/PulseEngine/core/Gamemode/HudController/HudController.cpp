#include "HudController.h"
#include "PulseEngine/core/Gamemode/HudController/Widget.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/TextComponent/TextComponent.h"

PULSE_REGISTER_CLASS_CPP(HudController)

void HudController::Serialize(Archive& ar)
{

}
void HudController::Deserialize(Archive& ar)
{

}
const char* HudController::ToString()
{
    return "HudController";
}


void HudController::Init()
{
}

void HudController::Update()
{
    for(Widget* wid : widgets)
    {
        wid->Update();
    }
}

void HudController::Render()
{
    for(Widget* wid : widgets)
    {
        wid->Render();
    }
}


