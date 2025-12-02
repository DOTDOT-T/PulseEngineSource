#include "TextComponent.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Graphics/TextRenderer.h"

#ifdef ENGINE_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#endif

PULSE_REGISTER_CLASS_CPP(TextComponent)

TextComponent::TextComponent() : WidgetComponent("Textcomponent", PulseEngine::Vector2(0.0f,0.0f))
{
}

TextComponent::TextComponent(const char *txt, PulseEngine::Vector2 location, PulseEngine::Vector2 anch) : WidgetComponent("textcomponent", location, anch), text(txt)
{
    tr = PulseEngineInstance->graphicsAPI->CreateNewText();
    tr->Init();
    tr->SetScreenSize(1920, 1080);
}

void TextComponent::SpecialWidgetDisplayEditor()
{
#ifdef ENGINE_EDITOR
    static char buffer[256];
    strncpy(buffer, text.c_str(), sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';

    if (ImGui::InputText("Text", buffer, sizeof(buffer))) 
    {
        text = buffer;
    }

    float colorList[3] = {color.r, color.g, color.b};
    if(ImGui::InputFloat3("Color text", &colorList[0]))
    {
        color.r = colorList[0];
        color.g = colorList[1];
        color.b = colorList[2];
    }
    if(ImGui::InputFloat("Font size", &fontSize))
    {

    }
#endif
}

void TextComponent::Update()
{
}

void TextComponent::Render()
{
    float screenWidth  = *PulseEngineInstance->graphicsAPI->width;
    float screenHeight = *PulseEngineInstance->graphicsAPI->height;

    // anchor in screen space
    float anchorX = anchor.x * screenWidth;
    float anchorY = anchor.y * screenHeight;

    // pivot offset in widget units
    float pivotX = pivot.x * size.x;
    float pivotY = pivot.y * size.y;

    // final screen position
    float screenX = location.x + anchorX - pivotX;
    float screenY = location.y + anchorY - pivotY;

    tr->RenderText(text, screenX, screenY, fontSize, PulseEngine::Vector3(color.r, color.g, color.b));
    tr->Render();
}

