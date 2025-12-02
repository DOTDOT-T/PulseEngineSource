#include "TextComponent.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Graphics/TextRenderer.h"

PULSE_REGISTER_CLASS_CPP(TextComponent)

TextComponent::TextComponent()
{
}

TextComponent::TextComponent(const char *txt, PulseEngine::Vector2 location, PulseEngine::Vector2 anch) : WidgetComponent(location, anch), text(txt)
{
    tr = PulseEngineInstance->graphicsAPI->CreateNewText();
    tr->Init();
    tr->SetScreenSize(1920, 1080);
}

void TextComponent::Update()
{
}

void TextComponent::Render()
{
    float screenX = *PulseEngineInstance->graphicsAPI->width  * anchor.x + transform.position.x;
    float screenY = *PulseEngineInstance->graphicsAPI->height * anchor.y + transform.position.y;

    tr->RenderText(text, 0, 25, 25.0f, PulseEngine::Vector3(color.r, color.g, color.b));
    tr->Render();
}
