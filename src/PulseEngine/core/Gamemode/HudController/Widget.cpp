#include "Widget.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/WidgetComponent.h"

PULSE_REGISTER_CLASS_CPP(Widget)

void Widget::Serialize(Archive& ar)
{

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
