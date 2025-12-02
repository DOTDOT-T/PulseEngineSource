#ifndef __WIDGETCOMPONENT_H__
#define __WIDGETCOMPONENT_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Math/Transform/Transform.h"

class PULSE_ENGINE_DLL_API WidgetComponent : public PulseObject
{
PULSE_GEN_BODY(WidgetComponent)
PULSE_REGISTER_CLASS_HEADER(WidgetComponent)

public:
    // --- Constructors --------------------------------------------------------
    WidgetComponent();
    WidgetComponent(
        const std::string& name,
        const PulseEngine::Vector2& location,
        const PulseEngine::Vector2& anch = PulseEngine::Vector2(0.0f, 0.0f)
    );

    virtual ~WidgetComponent() = default;

    // --- Core lifecycle ------------------------------------------------------
    virtual void Update();     // Editor + Runtime
    virtual void Render();     // UI Rendering

    // --- Hierarchy -----------------------------------------------------------
    void AddChild(WidgetComponent* child);
    void RemoveChild(WidgetComponent* child);
    WidgetComponent* GetParent() const { return parent; }
    const std::vector<WidgetComponent*>& GetChildren() const { return children; }

    // --- Transform helpers ---------------------------------------------------
    PulseEngine::Vector2 ComputeLocalPosition() const;
    PulseEngine::Vector2 ComputeAnchoredPosition(const PulseEngine::Vector2& parentSize) const;
    PulseEngine::Vector2 ComputePivotOffset() const;
    PulseEngine::Vector2 ComputeFinalPosition(const PulseEngine::Vector2& parentSize) const;

    // --- Accessors -----------------------------------------------------------
    void SetLocation(const PulseEngine::Vector2& loc) { location = loc; }
    void SetSize(const PulseEngine::Vector2& s) { size = s; }
    void SetColor(const PulseEngine::Vector4& c) { color = c; }
    void SetAnchor(const PulseEngine::Vector2& a) { anchor = a; }
    void SetPivot(const PulseEngine::Vector2& p) { pivot = p; }

    const PulseEngine::Vector2& GetLocation() const { return location; }
    const PulseEngine::Vector2& GetSize() const { return size; }
    const PulseEngine::Vector4& GetColor() const { return color; }
    const PulseEngine::Vector2& GetAnchor() const { return anchor; }
    const PulseEngine::Vector2& GetPivot() const { return pivot; }

    virtual void SpecialWidgetDisplayEditor();


    // --- Local space (relative to anchor) -----------------------------------
    PulseEngine::Vector2 location = PulseEngine::Vector2(0, 0);
    PulseEngine::Vector2 size     = PulseEngine::Vector2(100, 40);
    PulseEngine::Vector4 color    = PulseEngine::Vector4(1, 1, 1, 1);

    // --- Layout settings -----------------------------------------------------
    PulseEngine::Vector2 anchor = PulseEngine::Vector2(0, 0);   // normalized (0..1)
    PulseEngine::Vector2 pivot  = PulseEngine::Vector2(0, 0);   // normalized (0..1)

    // --- Hierarchy -----------------------------------------------------------
    WidgetComponent* parent = nullptr;
    std::vector<WidgetComponent*> children;

    std::string name;
};

#endif // __WIDGETCOMPONENT_H__
