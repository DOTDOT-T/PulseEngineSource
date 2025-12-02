#ifndef __WIDGETEDITOR_H__
#define __WIDGETEDITOR_H__

#include "common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/WidgetComponent.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <vector>

class PULSE_ENGINE_DLL_API WidgetEditor : public PulseObject
{
PULSE_GEN_BODY(WidgetEditor)
PULSE_REGISTER_CLASS_HEADER(WidgetEditor)

public:
    void Update();
    void Render();


private:
    // Panels
    void RenderDockspace();
    void RenderPalette();
    void RenderHierarchy();
    void RenderDetails();
    void RenderCanvas();

    // Canvas subsystems
    void Canvas_DrawBackground(const ImVec2& pos, const ImVec2& size);
    void Canvas_DrawWidgets(const ImVec2& pos, const ImVec2& size);
    void Canvas_HandleSelection(const ImVec2& pos, const ImVec2& size);
    void Canvas_HandleDragging(const ImVec2& pos, const ImVec2& size);

    // Positioning
    ImVec2 ComputeWidgetCanvasPos(const WidgetComponent* w, const ImVec2& parentSize);
    ImVec2 ScreenToCanvas(const ImVec2& screen, const ImVec2& canvasOrigin);
    ImVec2 CanvasToScreen(const ImVec2& canvas, const ImVec2& canvasOrigin);

private:
    std::vector<WidgetComponent*> widgets;
    WidgetComponent* selectedWidget = nullptr;


    ImVec2 dragStart  = ImVec2(0,0);
    ImVec2 widgetStart = ImVec2(0,0);

    bool isDragging = false;

    // canvas transforms
    ImVec2 canvasPan = ImVec2(0,0);
    float canvasZoom = 1.0f;

    // grid
    float gridSize = 16.0f;
    bool showGrid = true;
    bool snapToGrid = true;
};

#endif
