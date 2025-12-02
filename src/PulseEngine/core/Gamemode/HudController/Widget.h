#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

class WidgetComponent;

class PULSE_ENGINE_DLL_API Widget : public PulseObject
{
PULSE_GEN_BODY(Widget)
PULSE_REGISTER_CLASS_HEADER(Widget)
public:
    Widget() = default;
    virtual void Update();
    virtual void Render();

    void AddComponent(WidgetComponent* comp);

    void SetComponent(std::vector<WidgetComponent*> comp);

    std::vector<WidgetComponent*>* GetWidgets();
private:
    std::vector<WidgetComponent*> component;
};

#endif // __WIDGET_H__