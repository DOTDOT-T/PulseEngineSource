#include "PulseEngineEditor/InterfaceEditor/Synapse/Synapse.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/NodeMenuRegistry.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/Node.h"
// #include "imgui/imgui.h"
#include "Synapse.h"

namespace ed = ax::NodeEditor;

    Synapse::Synapse(const std::string &synapsePath)
{
}

void Synapse::Init()

{
    if (!context)
    {
        ed::Config config;
        config.SettingsFile = "NodeEditor.json";
        context = ed::CreateEditor(&config);
    }
}

void Synapse::Render()
{
    ImGui::Begin("Synapse", nullptr,
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoBringToFrontOnFocus);

    ed::SetCurrentEditor(context);

    // --- Main content area ---
    float bottomBarHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 contentSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - bottomBarHeight);

    ImGui::BeginChild("NodeEditorArea", contentSize, false);
    {
        ed::Begin("My Node Editor");
        // Your node rendering logic here
        ed::End();
    }
    ImGui::EndChild();

    // --- Bottom bar with menu ---
    ImGui::Separator();
    ImGui::BeginChild("BottomBar", ImVec2(0, bottomBarHeight), false,
        ImGuiWindowFlags_MenuBar); // enables menu bar inside this child

    if (ImGui::BeginMenuBar())
    {
        RenderNodeMenu(NodeMenuRegistry::Get().GetRootCategories());
        ImGui::EndMenuBar();
    }

    ImGui::EndChild();

    ed::SetCurrentEditor(nullptr);
    ImGui::End();
}

void Synapse::RenderNodeMenu(const std::vector<NodeCategory>& categories)
{
    for (auto& cat : categories)
    {
        if (ImGui::BeginMenu(cat.name.c_str()))
        {
            for (auto& node : cat.nodes)
            {
                if (ImGui::MenuItem(node.name.c_str()))
                {
                    node.onClick();
                }
            }

            // Recursively render subcategories
            if (!cat.subCategories.empty())
                RenderNodeMenu(cat.subCategories);

            ImGui::EndMenu();
        }
    }
}