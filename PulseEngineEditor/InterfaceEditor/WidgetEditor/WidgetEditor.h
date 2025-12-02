#ifndef __WIDGETEDITOR_H__
#define __WIDGETEDITOR_H__


#include "common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui-node/imgui_node_editor.h"
#include "imgui-node/imgui_node_editor_internal.h"
namespace ed = ax::NodeEditor;

class PULSE_ENGINE_DLL_API WidgetEditor : public PulseObject
{
PULSE_GEN_BODY(WidgetEditor)
PULSE_REGISTER_CLASS_HEADER(WidgetEditor)
public:
    void Update();
    void Render();

        struct Widget
    {
        std::string name;
        ImVec2 pos;      // position relative to the canvas (top-left)
        ImVec2 size;
        ImVec4 color;

        Widget(const char* n, ImVec2 p, ImVec2 s)
            : name(n), pos(p), size(s), color(1,1,1,1) {}
    };

    std::vector<std::shared_ptr<Widget>> widgets;

    Widget* selectedWidget = nullptr;

    // dragging variables
    ImVec2 dragStart     = ImVec2(0,0);
    ImVec2 widgetStart   = ImVec2(0,0);
private:

};

#endif // __WIDGETEDITOR_H__