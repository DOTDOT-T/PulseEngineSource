#include "WidgetEditor.h"
#include "PulseEngine/core/Gamemode/Gamemode.h"
#include "PulseEngine/core/Gamemode/HudController/HudController.h"
#include "PulseEngine/core/Gamemode/HudController/Widget.h"
#include "PulseEngine/core/Gamemode/HudController/WidgetComponent/TextComponent/TextComponent.h"
#include "PulseEngine/core/FileManager/Archive/DiskArchive.h"

PULSE_REGISTER_CLASS_CPP(WidgetEditor)

void WidgetEditor::Update()
{
}

const char *WidgetEditor::ToString() { return "Widget Editor"; }
void WidgetEditor::Serialize(Archive &ar) {}
void WidgetEditor::Deserialize(Archive &ar) {}

// ------------------------------------------------------------
// Main Rendering Entry
// ------------------------------------------------------------
void WidgetEditor::Render()
{
    Update();

    RenderDockspace();
    RenderPalette();
    RenderDetails();
    RenderHierarchy();
    RenderCanvas();
}

void WidgetEditor::OpenNewWidget(const std::string& newPath)
{
    SaveActualWidget();

    delete userSelectedWidget;
    userSelectedWidget = nullptr;

    DiskArchive loadFromDar(newPath, Archive::Mode::Loading);
    userSelectedWidget = new Widget();
    pathToWidget = newPath;
    widgets = userSelectedWidget->GetWidgets();
    EDITOR_INFO("New widget opened " << pathToWidget)


    if(!loadFromDar.IsArchiveEmpty()) userSelectedWidget->Serialize(loadFromDar);
}

void WidgetEditor::SaveActualWidget()
{
    DiskArchive saveToDar(pathToWidget, Archive::Mode::Saving);
    if(!saveToDar.IsArchiveOpen()) return;
    userSelectedWidget->Serialize(saveToDar);
    saveToDar.Finalize();
}

// ------------------------------------------------------------
// Dockspace (UE-like workspace)
// ------------------------------------------------------------
void WidgetEditor::RenderDockspace()
{
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::Begin("WidgetEditor", nullptr, flags);
    ImGuiID dock_id = ImGui::GetID("WidgetEditorDockspace");
    ImGui::DockSpace(dock_id);
    ImGui::End();
}

// ------------------------------------------------------------
// Palette
// ------------------------------------------------------------
void WidgetEditor::RenderPalette()
{
    ImGui::Begin("Palette");
    ImGui::Text("Widgets");
    ImGui::Separator();

    if (ImGui::Selectable("Button"))
        widgets->push_back(new WidgetComponent("Button", PulseEngine::Vector2(100, 100), PulseEngine::Vector2(120, 40)));

    if (ImGui::Selectable("Text"))
        widgets->push_back(new TextComponent("Text", PulseEngine::Vector2(200, 200), PulseEngine::Vector2(100, 20)));

    if (ImGui::Selectable("Image"))
        widgets->push_back(new WidgetComponent("Image", PulseEngine::Vector2(150, 150), PulseEngine::Vector2(80, 80)));

    ImGui::End();
}

// ------------------------------------------------------------
// Details
// ------------------------------------------------------------
void WidgetEditor::RenderDetails()
{
    ImGui::Begin("Details");

    if (ImGui::Button("Save"))
    {
        SaveActualWidget();
    }
    if (selectedWidget)
    {
        ImGui::Text("Details");
        ImGui::Separator();
        static char buffer[256];
        strncpy(buffer, selectedWidget->name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            selectedWidget->name = buffer;
        }
        ImGui::InputFloat2("Position", &selectedWidget->location.x);
        ImGui::InputFloat2("Size", &selectedWidget->size.x);
        ImGui::ColorEdit4("Tint", &selectedWidget->color.x);
        ImGui::InputFloat2("Anchor", &selectedWidget->anchor.x);
        ImGui::SliderFloat2("Pivot", &selectedWidget->pivot.x, 0.0f, 1.0f);

        ImGui::Text("Widget unique details");
        ImGui::Separator();
        selectedWidget->SpecialWidgetDisplayEditor();
    }
    else
        ImGui::Text("Select a widget.");

    ImGui::End();
}

// ------------------------------------------------------------
// Hierarchy
// ------------------------------------------------------------
void WidgetEditor::RenderHierarchy()
{
    ImGui::Begin("Hierarchy");
    ImGui::Text("Widget Tree");
    ImGui::Separator();

    if (widgets)
    {
        for (auto &w : *widgets)
        {
            bool sel = (w == selectedWidget);
            if (ImGui::Selectable(w->name.c_str(), sel))
                selectedWidget = w;
        }
    }

    ImGui::End();
}

// ------------------------------------------------------------
// Canvas
// ------------------------------------------------------------
void WidgetEditor::RenderCanvas()
{
    ImGui::Begin("Designer");

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    Canvas_DrawBackground(canvasPos, canvasSize);
    Canvas_DrawWidgets(canvasPos, canvasSize);

    if (ImGui::IsWindowHovered())
    {
        Canvas_HandleSelection(canvasPos, canvasSize);
        Canvas_HandleDragging(canvasPos, canvasSize);
    }

    ImGui::End();
}

void WidgetEditor::Canvas_DrawBackground(const ImVec2 &canvasPos, const ImVec2 &canvasSize)
{
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(45, 45, 45, 255));
    if (!showGrid)
        return;

    float scaledGrid = gridSize * canvasZoom;

    // protect against division by zero / too small zoom
    if (scaledGrid < 4.0f)
        return;

    const float xStart = canvasPos.x + fmodf(canvasPan.x, scaledGrid);
    const float yStart = canvasPos.y + fmodf(canvasPan.y, scaledGrid);

    ImU32 gridColor = IM_COL32(60, 60, 60, 180);

    // vertical lines
    for (float x = xStart; x < canvasPos.x + canvasSize.x; x += scaledGrid)
        dl->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasPos.y + canvasSize.y), gridColor);

    // horizontal lines
    for (float y = yStart; y < canvasPos.y + canvasSize.y; y += scaledGrid)
        dl->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasPos.x + canvasSize.x, y), gridColor);
}

void WidgetEditor::Canvas_DrawWidgets(const ImVec2 &canvasPos, const ImVec2 &canvasSize)
{
    ImDrawList *dl = ImGui::GetWindowDrawList();
    if (widgets)
    {
        for (auto &w : *widgets)
        {
            ImVec2 local = ComputeWidgetCanvasPos(w, canvasSize);
            ImVec2 p0 = CanvasToScreen(local, canvasPos);
            ImVec2 p1 = CanvasToScreen(ImVec2(local.x + w->size.x, local.y + w->size.y), canvasPos);

            ImU32 color = (w == selectedWidget)
                              ? IM_COL32(255, 120, 120, 255)
                              : IM_COL32(210, 210, 210, 255);

            dl->AddRect(p0, p1, color, 2.0f);
            dl->AddText(ImVec2(p0.x + 4, p0.y + 4), IM_COL32_WHITE, w->name.c_str());
        }
    }
}
void WidgetEditor::Canvas_HandleSelection(const ImVec2 &canvasPos, const ImVec2 &canvasSize)
{
    if (!ImGui::IsMouseClicked(0))
        return;

    ImVec2 mouseCanvas = ScreenToCanvas(ImGui::GetIO().MousePos, canvasPos);

    selectedWidget = nullptr;
    if (widgets)
    {
        for (auto &w : *widgets)
        {
            ImVec2 local = ComputeWidgetCanvasPos(w, canvasSize);

            ImVec2 min = local;
            ImVec2 max(min.x + w->size.x, min.y + w->size.y);

            if (mouseCanvas.x >= min.x && mouseCanvas.x <= max.x &&
                mouseCanvas.y >= min.y && mouseCanvas.y <= max.y)
            {
                selectedWidget = w;
                dragStart = mouseCanvas;
                widgetStart = ImVec2(w->location.x, w->location.y);
                break;
            }
        }
    }
}

void WidgetEditor::Canvas_HandleDragging(const ImVec2 &canvasPos, const ImVec2 &canvasSize)
{
    if (!selectedWidget)
        return;

    if (!ImGui::IsMouseDragging(0))
        return;

    ImVec2 mouseCanvas = ScreenToCanvas(ImGui::GetIO().MousePos, canvasPos);
    ImVec2 delta = mouseCanvas - dragStart;

    ImVec2 newPos = widgetStart + delta;

    if (snapToGrid)
    {
        newPos.x = roundf(newPos.x / gridSize) * gridSize;
        newPos.y = roundf(newPos.y / gridSize) * gridSize;
    }

    selectedWidget->location = PulseEngine::Vector2(newPos.x, newPos.y);
}

ImVec2 WidgetEditor::ComputeWidgetCanvasPos(const WidgetComponent *w, const ImVec2 &parentSize)
{
    ImVec2 anchorOffset(
        w->anchor.x * parentSize.x,
        w->anchor.y * parentSize.y);

    ImVec2 local = ImVec2(w->location.x + anchorOffset.x, w->location.y + anchorOffset.y);

    ImVec2 pivotOffset(
        w->pivot.x * w->size.x,
        w->pivot.y * w->size.y);

    return local - pivotOffset;
}

ImVec2 WidgetEditor::ScreenToCanvas(const ImVec2 &screen, const ImVec2 &origin)
{
    return ImVec2(
        (screen.x - origin.x - canvasPan.x) / canvasZoom,
        (screen.y - origin.y - canvasPan.y) / canvasZoom);
}

ImVec2 WidgetEditor::CanvasToScreen(const ImVec2 &canvas, const ImVec2 &origin)
{
    return ImVec2(
        origin.x + canvasPan.x + canvas.x * canvasZoom,
        origin.y + canvasPan.y + canvas.y * canvasZoom);
}
