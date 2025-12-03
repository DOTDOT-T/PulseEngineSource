#include "WidgetComponent.h"
#include <algorithm>
#include "PulseEngine/core/FileManager/Archive/Archive.h"

WidgetComponent::WidgetComponent()
{
}

WidgetComponent::WidgetComponent(
    const std::string& name,
    const PulseEngine::Vector2& location,
    const PulseEngine::Vector2& anch
)
    : location(location), anchor(anch), name(name)
{
}

void WidgetComponent::Serialize(Archive& ar)
{
    ar.Serialize("location.x", location.x);
    ar.Serialize("location.y", location.y);
    ar.Serialize("location.z", location.z);
    ar.Serialize("size.x", size.x);
    ar.Serialize("size.y", size.y);
    ar.Serialize("widgetColor.x", color.x);
    ar.Serialize("widgetColor.y", color.y);
    ar.Serialize("widgetColor.z", color.z);
    ar.Serialize("anchor.x", anchor.x);
    ar.Serialize("anchor.y", anchor.y);
    ar.Serialize("pivot.x", pivot.x);
    ar.Serialize("pivot.y", pivot.y);
    for(auto& wdg : children)
    {
        wdg->Serialize(ar);
    }
}
void WidgetComponent::Deserialize(Archive& ar) 
{

}
const char* WidgetComponent::ToString() 
{
    return "WidgetComp";
}



void WidgetComponent::AddChild(WidgetComponent* child)
{
    if (!child) return;

    children.push_back(child);
    child->parent = this;
}

void WidgetComponent::RemoveChild(WidgetComponent* child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

void WidgetComponent::Update()
{
    // override in derived widgets
}

void WidgetComponent::Render()
{
    // override in derived widgets
}

PulseEngine::Vector2 WidgetComponent::ComputeLocalPosition() const
{
    return location;
}

PulseEngine::Vector2 WidgetComponent::ComputeAnchoredPosition(
    const PulseEngine::Vector2& parentSize
) const
{
    PulseEngine::Vector2 anchorOffset(
        parentSize.x * anchor.x,
        parentSize.y * anchor.y
    );

    return PulseEngine::Vector2(location.x + anchorOffset.x, location.y + anchorOffset.y);
}

PulseEngine::Vector2 WidgetComponent::ComputePivotOffset() const
{
    return PulseEngine::Vector2(
        size.x * pivot.x,
        size.y * pivot.y
    );
}

PulseEngine::Vector2 WidgetComponent::ComputeFinalPosition(
    const PulseEngine::Vector2& parentSize
) const
{
    return ComputeAnchoredPosition(parentSize) - ComputePivotOffset();
}

void WidgetComponent::SpecialWidgetDisplayEditor()
{
}
