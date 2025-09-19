@echo off
echo === Building PulseEditor.dll (Editor + ImGui) ===

g++ -shared -Wl,--out-implib,PulseEngineBuild/libPulseEditor.a ^
-o PulseEngineBuild/PulseEditor.dll ^
src/main.cpp ^
src/glad.c ^
src/shader.cpp ^
src/camera.cpp ^
PulseEngineEditor/InterfaceEditor/InterfaceEditor.cpp ^
PulseEngineEditor/InterfaceEditor/TopBar.cpp ^
PulseEngineEditor/InterfaceEditor/BuildGameCoroutine.cpp ^
src/PulseEngine/core/coroutine/CoroutineManager.cpp ^
src/PulseEngine/CustomScripts/ScriptsLoader.cpp ^
imgui/imgui.cpp ^
imgui/imgui_draw.cpp ^
imgui/imgui_tables.cpp ^
imgui/imgui_widgets.cpp ^
imgui/imgui_demo.cpp ^
imgui/backends/imgui_impl_glfw.cpp ^
imgui/backends/imgui_impl_opengl3.cpp ^
-I. -Iinclude -Iimgui -I./src -IE:/Editor/Include ^
-LPulseEngineBuild -Lexternal/assimp/lib -Lexternal/assimp/lib/x64 -LD:/MSYS2/mingw64/lib ^
-lPulseEngine -lglfw3 -lopengl32 -lm -lassimp ^
-DBUILDING_DLL -DENGINE_EDITOR -DPULSE_EXPORT_OPENGL

if %errorlevel% neq 0 (
    echo Editor build failed!
    exit /b %errorlevel%
) else (
    echo Editor build succeeded!
)
