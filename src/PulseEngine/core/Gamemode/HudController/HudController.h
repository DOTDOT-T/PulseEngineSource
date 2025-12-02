#ifndef __HUDCONTROLLER_H__
#define __HUDCONTROLLER_H__


#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

class Widget;

class HudController : public PulseObject
{
PULSE_GEN_BODY(HudController)
PULSE_REGISTER_CLASS_HEADER(HudController)
public:
    void Init();
    void Update();
    void Render();

private:
    std::vector<Widget*> widgets;

};

#endif // __HUDCONTROLLER_H__