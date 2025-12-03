#include "ViewportRenderer.h"


#include  "PulseEngine/ModuleLoader/IModuleInterface/IModuleInterface.h"
#include  "PulseEngine/ModuleLoader/IModule/IModule.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceEditor.h"
#include <WinUser.h>

std::string Viewport::GetName() const
{
    return std::string("viewport");
}

std::string Viewport::GetVersion() const
{
    return std::string("0.0.1");
}

void Viewport::Initialize()
{
}

void Viewport::Shutdown()
{
}

void Viewport::Render()
{
    // PulseInterfaceAPI::GizmoDebug();
     PulseInterfaceAPI::OpenWindow("Viewport");

     SceneRenderer();

     if(PulseInterfaceAPI::Button(std::to_string((int)top).c_str(), PulseEngine::Vector2(0.0f,0.0f)))
     {
        top = (TransformOperator)((((int)top) + 1) % 4);
     }
     PulseInterfaceAPI::CloseWindow();
}

void Viewport::SceneRenderer()
{
    if (PulseInterfaceAPI::IsCurrentWindowFocused() && (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
    {
        if (GetAsyncKeyState('W') & 0x8000)
            PulseEngineInstance->GetActiveCamera()->ProcessKeyboard(FORWARD, PulseEngineInstance->GetDeltaTime());
        if (GetAsyncKeyState('S') & 0x8000)
            PulseEngineInstance->GetActiveCamera()->ProcessKeyboard(BACKWARD, PulseEngineInstance->GetDeltaTime());
        if (GetAsyncKeyState('A') & 0x8000)
            PulseEngineInstance->GetActiveCamera()->ProcessKeyboard(LEFT, PulseEngineInstance->GetDeltaTime());
        if (GetAsyncKeyState('D') & 0x8000)
            PulseEngineInstance->GetActiveCamera()->ProcessKeyboard(RIGHT, PulseEngineInstance->GetDeltaTime());
        PulseEngineInstance->GetActiveCamera()->ProcessMouseMovement(PulseInterfaceAPI::MouseDelta().x, -PulseInterfaceAPI::MouseDelta().y);
    }

    PulseEngine::Vector2 windowSize = PulseInterfaceAPI::GetActualWindowSize();
    windowSize.x -= 100.0f;
    windowSize.y -= 100.0f;

    PulseEngine::Vector2 screenPos = PulseInterfaceAPI::GetCursorScreenPos();

    PulseInterfaceAPI::RenderCameraToInterface(&previewData, PulseEngineInstance->GetActiveCamera(), "Viewport", windowSize, PulseEngineInstance->entities);
    if (auto* gizmo = PulseInterfaceAPI::GetSelectedGizmo())
    {
        PulseInterfaceAPI::SetCursorScreenPos(screenPos);
        PulseInterfaceAPI::RenderGizmo(gizmo, windowSize, top);
    }
}
