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
            std::string typeName = component[i]->GetTypeName();
            EDITOR_INFO("Serialize of a widgetComponent[" << typeName << "]")
            ar.Serialize("TypeName", typeName);
            component[i]->Serialize(ar);
        }
        else
        {
            WidgetComponent* cmp;
            std::string typeName;
            ar.Serialize("TypeName", typeName);
            cmp = TypeRegistry::CreateInstance<WidgetComponent>(typeName);
            if(!cmp)
            {
                EDITOR_ERROR("WidgetComponent[" << typeName << "] couldn't be found in engine-type-register. Couldn't load widget safely.")
                EDITOR_INFO("To solve : " << std::endl << 
                    "- Delete HUD and recreate it from zero." << std::endl <<
                    "- If WidgetComponent[" << typeName << "] was a custom, recreate it with the SAME name.")
                return;
            }

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
