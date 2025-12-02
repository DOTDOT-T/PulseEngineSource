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
    Widget* wgt = new Widget();
    wgt->AddComponent(new TextComponent("SIMPLE TEXT", PulseEngine::Vector2(10.0f,10.0f)));
    widgets.push_back(wgt);
    std::cout << "init new text in widget" << std::endl;
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
