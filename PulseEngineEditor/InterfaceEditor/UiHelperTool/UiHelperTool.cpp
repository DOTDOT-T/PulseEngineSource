#include "UiHelperTool.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include <imgui/imgui.h>

void UiHelperTool::DisplayScriptEditor(IScript *script, int scriptIndex)
{
    // --- Header (UE-like section collapsible) ---
    std::string headerLabel = script->GetName() + std::string("###") + script->GetName() + "_" + std::to_string(scriptIndex);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.16f, 0.16f, 0.17f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.23f, 0.23f, 0.25f, 1.0f));

    bool open = ImGui::TreeNodeEx(
        headerLabel.c_str(),
        ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_DefaultOpen,
        "%s", script->GetName()
    );

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    if (!open)
        return;

    // Inner indent
    ImGui::Indent(12.0f);
    ImGui::Spacing();

    if (!script->isEntityLinked)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.45f, 0.2f, 1.0f));
        ImGui::Text("🎯 Only on this entity");
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    // Small separator like UE attributes
    ImGui::Separator();
    ImGui::Spacing();

    // --- Exposed Variables ---
    int varCounter = 0;
    auto exposedVars = script->GetExposedVariables();

    for (auto& var : exposedVars)
    {
        ImGui::PushID(varCounter);

        float fullWidth = ImGui::GetContentRegionAvail().x;
        float labelWidth = fullWidth * 0.35f;
        float fieldWidth = fullWidth - labelWidth - 10.0f;

        // Label column
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.90f, 1.0f));
        ImGui::Text("%s", var.name.c_str());
        ImGui::PopStyleColor();

        ImGui::SameLine(labelWidth);

        // Field column
        ImGui::SetNextItemWidth(fieldWidth);

        // Unreal-like input zone background
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.11f, 0.11f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.14f, 0.14f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.15f, 0.15f, 0.16f, 1.0f));

        switch (var.type)
        {
        case ExposedVariable::Type::INT:
            ImGui::DragInt("##int", reinterpret_cast<int*>(var.ptr), 1.0f);
            break;

        case ExposedVariable::Type::FLOAT:
            ImGui::DragFloat("##float", reinterpret_cast<float*>(var.ptr), 0.05f);
            break;

        case ExposedVariable::Type::FLOAT3:
            ImGui::DragFloat3("##float3", reinterpret_cast<float*>(var.ptr), 0.05f);
            break;

        case ExposedVariable::Type::BOOL:
            ImGui::Checkbox("##bool", reinterpret_cast<bool*>(var.ptr));
            break;

        case ExposedVariable::Type::STRING:
            ImGui::InputText("##string", reinterpret_cast<char*>(var.ptr), 256);
            break;
        }

        ImGui::PopStyleColor(3); // frame colors

        ImGui::Spacing();
        ImGui::Separator();      // thin line between properties
        ImGui::Spacing();

        ImGui::PopID();
        varCounter++;
    }

    ImGui::Unindent(12.0f);
    ImGui::TreePop();
}
ContextMenuItem UiHelperTool::GenerateContextHeader(const char* headerName)
{
    ContextMenuItem header;
    header.label = headerName;

    header.customDisplay = [headerName]()
    {
        // --- Unreal Slate Color Palette ---
        const ImVec4 BackgroundColor = ImVec4(0.16f, 0.16f, 0.17f, 1.0f); // subtle dark highlight
        const ImVec4 TextColor       = ImVec4(0.82f, 0.82f, 0.84f, 1.0f); // bright slate text
        const ImVec4 AccentColor     = ImVec4(0.08f, 0.38f, 0.75f, 1.0f); // UE blue
        const ImVec4 LineColor       = ImVec4(0.10f, 0.10f, 0.11f, 1.0f); // shadow line

        // --- Layout ---
        const float HeaderHeight = 24.0f;
        const float AccentWidth  = 2.0f;
        const float TextPadX     = 8.0f;
        const float TextPadY     = 5.5f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 pos  = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, HeaderHeight);

        // --- Background (light highlight like Slate sections) ---
        drawList->AddRectFilled(
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImGui::ColorConvertFloat4ToU32(BackgroundColor)
        );

        // --- Left Accent (vertical UE strip) ---
        drawList->AddRectFilled(
            ImVec2(pos.x, pos.y),
            ImVec2(pos.x + AccentWidth, pos.y + size.y),
            ImGui::ColorConvertFloat4ToU32(AccentColor)
        );

        // --- Header label ---
        ImGui::SetCursorScreenPos(ImVec2(pos.x + AccentWidth + TextPadX, pos.y + TextPadY));
        ImGui::PushStyleColor(ImGuiCol_Text, TextColor);
        ImGui::TextUnformatted(headerName);
        ImGui::PopStyleColor();

        // --- Bottom shadow line (UE/Slate style) ---
        drawList->AddLine(
            ImVec2(pos.x, pos.y + size.y - 1),
            ImVec2(pos.x + size.x, pos.y + size.y - 1),
            ImGui::ColorConvertFloat4ToU32(LineColor),
            1.0f
        );

        // Reserve the layout space
        ImGui::Dummy(size);
    };

    header.type = EditorWidgetComponent::EMPTY;
    return header;
}


