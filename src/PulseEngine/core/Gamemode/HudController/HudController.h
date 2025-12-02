#ifndef __HUDCONTROLLER_H__
#define __HUDCONTROLLER_H__


#include "Common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Gamemode/HudController/Widget.h"
#include <vector>

class PULSE_ENGINE_DLL_API HudController : public PulseObject
{
PULSE_GEN_BODY(HudController)
PULSE_REGISTER_CLASS_HEADER(HudController)
public:
    void Init();
    void Update();
    void Render();

    Widget* At(int index)
    {
        if(index >= widgets.size())
            widgets.push_back(new Widget());
        return widgets[index];
    }

private:
    std::vector<Widget*> widgets;

};

#endif // __HUDCONTROLLER_H__