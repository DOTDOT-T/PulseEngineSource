#include "WidgetComponent.h"

PULSE_REGISTER_CLASS_CPP(WidgetComponent)

void WidgetComponent::Serialize(Archive& ar)
{

}
void WidgetComponent::Deserialize(Archive& ar)
{

}
const char* WidgetComponent::ToString()
{
    return "Widget Component";
}

WidgetComponent::WidgetComponent()
{
}

WidgetComponent::WidgetComponent(PulseEngine::Vector2 location, PulseEngine::Vector2 anch) : anchor(anch)
{
    transform.position = PulseEngine::Vector3(location.x, location.y, 0.0f);
}

void WidgetComponent::Update()
{
}

void WidgetComponent::Render()
{
}
