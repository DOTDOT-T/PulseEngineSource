#include "NativeInit.h"
#include "PulseEngine/core/PulseScript/PulseInterpreter.h"
#include "common/EditorDefines.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui-node/imgui_node_editor.h"
#include "imgui-node/imgui_node_editor_internal.h"
namespace ed = ax::NodeEditor;

void InitNativeMethods()
{    
    PulseInterpreter::RegisterFunction("Log",
        [](const std::vector<Value> &args) -> Value
        {
            std::ostringstream oss;

            for (const auto& v : args)
            {
                std::visit([&oss](auto&& val) {
                    oss << val << " ";
                }, v);
            }

            EDITOR_LOG(oss.str()); // <-- toute la ligne dans le logger
            return 0;
        }
    ); 
    PulseInterpreter::RegisterFunction("Warn",
        [](const std::vector<Value> &args) -> Value
        {
            std::ostringstream oss;

            for (const auto& v : args)
            {
                std::visit([&oss](auto&& val) {
                    oss << val << " ";
                }, v);
            }

            EDITOR_WARN(oss.str()); // <-- toute la ligne dans le logger
            return 0;
        }
    );
    PulseInterpreter::RegisterFunction("Error",
        [](const std::vector<Value> &args) -> Value
        {
            std::ostringstream oss;

            for (const auto& v : args)
            {
                std::visit([&oss](auto&& val) {
                    oss << val << " ";
                }, v);
            }

            EDITOR_ERROR(oss.str()); // <-- toute la ligne dans le logger
            return 0;
        }
    );
    PulseInterpreter::RegisterFunction("OpenTool",
        [](const std::vector<Value> &args) -> Value
        {
            if (args.empty())
                throw std::runtime_error("ImGui.Begin expects a string argument");
        
            const std::string* title = std::get_if<std::string>(&args[0]);
            if (!title)
                throw std::runtime_error("ImGui.Begin argument must be a string");
        
            ImGui::Begin(title->c_str());
            return 0;
        }

    );
    PulseInterpreter::RegisterFunction("CloseTool",
        [](const std::vector<Value> &args) -> Value
        {
            ImGui::End();
            return 0;
        }
    );
}