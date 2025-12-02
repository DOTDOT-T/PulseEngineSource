#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

class Widget : public PulseObject
{
PULSE_GEN_BODY(Widget)
PULSE_REGISTER_CLASS_HEADER(Widget)
public:
    virtual Widget() = default;
    virtual void Update();
    virtual void Render();

private:
};

#endif // __WIDGET_H__