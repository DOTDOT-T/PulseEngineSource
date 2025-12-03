#include "HudController.h"
#include "PulseEngine/core/Gamemode/HudController/Widget.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/TextComponent/TextComponent.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

PULSE_REGISTER_CLASS_CPP(HudController)

void HudController::Serialize(Archive& ar)
{

    int size = widgets.size();
    ar.Serialize("size", size);
    for(unsigned int i = 0; i < size; ++i)
    {
        if(ar.IsSaving())
        {
            widgets[i]->Serialize(ar);
        }
        else
        {
            Widget* wdgt = new Widget();

            wdgt->Serialize(ar);
            widgets.push_back(wdgt);
        }
    }
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


