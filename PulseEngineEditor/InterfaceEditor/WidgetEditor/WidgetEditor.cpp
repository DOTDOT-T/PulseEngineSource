#include "WidgetEditor.h"

PULSE_REGISTER_CLASS_CPP(WidgetEditor)


void WidgetEditor::Serialize(Archive& ar)
{

}
void WidgetEditor::Deserialize(Archive& ar)
{

}
const char* WidgetEditor::ToString()
{
    return "Widget Editor";
}

void WidgetEditor::Update()
{
}

void WidgetEditor::Render()
{
    Update(); // your logic

    // --- DOCKING SETUP ---
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::Begin("WidgetEditor", nullptr, window_flags);

    // Main dockspace (like Unreal's workspace)
    ImGuiID dock_id = ImGui::GetID("WidgetEditorDockspace");
    ImGui::DockSpace(dock_id);

    ImGui::End();


    // --- LEFT: WIDGET PALETTE ---
    ImGui::Begin("Palette");
    ImGui::Text("Widgets");
    ImGui::Separator();

    if (ImGui::Selectable("Button"))
    {
        widgets.push_back(std::make_shared<Widget>("Button", ImVec2(100, 100), ImVec2(120, 40)));
    }
    if (ImGui::Selectable("Text"))
    {
        widgets.push_back(std::make_shared<Widget>("Text", ImVec2(200, 200), ImVec2(100, 20)));
    }
    if (ImGui::Selectable("Image"))
    {
        widgets.push_back(std::make_shared<Widget>("Image", ImVec2(150, 150), ImVec2(80, 80)));
    }


    ImGui::End();


    // --- RIGHT: DETAILS PANEL ---
    ImGui::Begin("Details");
    if (selectedWidget)
    {
        ImGui::Text("Details");
        ImGui::Separator();

        ImGui::InputFloat2("Position", &selectedWidget->pos.x);
        ImGui::InputFloat2("Size",     &selectedWidget->size.x);
        ImGui::ColorEdit4("Tint",      &selectedWidget->color.x);
    }
    else
    {
        ImGui::Text("Select a widget.");
    }
    ImGui::End();


    // --- LEFT BOTTOM: HIERARCHY ---
    ImGui::Begin("Hierarchy");
    ImGui::Text("Widget Tree");
    ImGui::Separator();

    for (auto& w : widgets)
    {
        bool selected = (w.get() == selectedWidget);
        if (ImGui::Selectable(w->name.c_str(), selected))
            selectedWidget = w.get();
    }
    ImGui::End();


    // --- CENTER: DESIGN CANVAS ---
    ImGui::Begin("Designer");

    ImVec2 canvasPos  = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    // background
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(canvasPos,
                      ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                      IM_COL32(45, 45, 45, 255));

    // --- draw widgets ---
    for (auto& w : widgets)
    {
        ImVec2 p = ImVec2(canvasPos.x + w->pos.x,
                          canvasPos.y + w->pos.y);
        ImVec2 s = ImVec2(p.x + w->size.x, p.y + w->size.y);

        ImU32 col = (w.get() == selectedWidget)
            ? IM_COL32(255, 100, 100, 255)
            : IM_COL32(200, 200, 200, 255);

        dl->AddRect(p, s, col, 2.0f);

        // draw name
        dl->AddText(ImVec2(p.x + 4, p.y + 4), IM_COL32_WHITE, w->name.c_str());
    }

    // --- input: click & drag ---
    if (ImGui::IsWindowHovered())
    {
        ImVec2 mouse = ImGui::GetIO().MousePos;

        // selection
        if (ImGui::IsMouseClicked(0))
        {
            selectedWidget = nullptr;

            for (auto& w : widgets)
            {
                ImVec2 p = ImVec2(canvasPos.x + w->pos.x,
                                  canvasPos.y + w->pos.y);
                ImVec2 s = ImVec2(p.x + w->size.x,
                                  p.y + w->size.y);

                if (mouse.x >= p.x && mouse.x <= s.x &&
                    mouse.y >= p.y && mouse.y <= s.y)
                {
                    selectedWidget = w.get();
                    dragStart = mouse;
                    widgetStart = w->pos;
                    break;
                }
            }
        }

        // drag selected
        if (selectedWidget && ImGui::IsMouseDragging(0))
        {
            ImVec2 delta = ImGui::GetIO().MousePos - dragStart;
            selectedWidget->pos = widgetStart + delta;
        }
    }

    ImGui::End();
}
