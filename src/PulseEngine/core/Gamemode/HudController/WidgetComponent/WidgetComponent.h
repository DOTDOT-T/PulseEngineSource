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
    WidgetComponent();
    WidgetComponent(PulseEngine::Vector2 location, PulseEngine::Vector2 anch = PulseEngine::Vector2(0.0f,0.0f));
    virtual void Update();
    virtual void Render();

protected:
    PulseEngine::Transform transform;
    //anchor are percent of the screen where the component origin should keep position
    PulseEngine::Vector2 anchor;


};


#endif // __WIDGETCOMPONENT_H__