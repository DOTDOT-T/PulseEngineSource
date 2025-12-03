#include "Widget.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/WidgetComponent.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

PULSE_REGISTER_CLASS_CPP(Widget)

void Widget::Serialize(Archive& ar)
{
    int size = component.size();
    ar.Serialize("size", size);
    for(unsigned int i = 0; i < size; ++i)
    {
        if(ar.IsSaving())
        {
            component[i]->Serialize(ar);
        }
        else
        {
            WidgetComponent* cmp = new WidgetComponent();

            cmp->Serialize(ar);
            component.push_back(cmp);
        }
    }
}
void Widget::Deserialize(Archive& ar)
{

}
const char* Widget::ToString()
{
    return "Widget";
}

void Widget::Update()
{
    for(auto& comp : component)
    {
        comp->Update();
    }
}

void Widget::Render()
{    
    for(auto& comp : component)
    {
        comp->Render();
    }
}

void Widget::AddComponent(WidgetComponent *comp)
{
    component.push_back(comp);
}

void Widget::SetComponent(std::vector<WidgetComponent *> comp)
{
    for(auto& c : component)
    {
        delete c;
    }
    component.clear();
    component = comp;
}

std::vector<WidgetComponent *> *Widget::GetWidgets()
{
    return &component;
}
