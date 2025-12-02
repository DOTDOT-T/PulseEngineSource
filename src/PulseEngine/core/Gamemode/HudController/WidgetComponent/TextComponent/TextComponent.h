#ifndef __TEXTCOMPONENT_H__
#define __TEXTCOMPONENT_H__

#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/WidgetComponent.h"

class ITextRenderer;

class PULSE_ENGINE_DLL_API TextComponent : public WidgetComponent
{
PULSE_REGISTER_CLASS_HEADER(TextComponent)
public:
    TextComponent();
    TextComponent(const char* txt, PulseEngine::Vector2 location, PulseEngine::Vector2 anch = PulseEngine::Vector2(0.0f,0.0f));

    void Update() override;
    void Render() override;

private:
    std::string text;
    ITextRenderer* tr;
    float size = 32.0f;
    PulseEngine::Color color = PulseEngine::Color(0.0f);

};

#endif // __TEXTCOMPONENT_H__