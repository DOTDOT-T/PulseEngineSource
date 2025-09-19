@echo off
echo === Building Pulse Engine Editor to .exe window ===

setlocal enabledelayedexpansion

set OBJFILES=
for %%f in (ObjectFiles\*.o) do (
    set OBJFILES=!OBJFILES! %%f
)


g++  -Isrc -Iinclude/imgui -Iinclude/imgui-node -Iimgui/backends -Iinclude -I. ^
%OBJFILES% ^
src/main.cpp ^
include/imgui/backends/imgui_impl_glfw.cpp ^
include/imgui/backends/imgui_impl_opengl3.cpp ^
include/imgui-node/imgui_node_editor.cpp ^
include/imgui-node/imgui_node_editor_api.cpp ^
include/imgui-node/imgui_canvas.cpp ^
include/imgui-node/crude_json.cpp ^
-LBuild -lPulseEngineEditor  ^
-LC:/path/to/glfw/lib ^
-lglfw3 -lopengl32 -lm -lassimp -lcomdlg32 -lws2_32 -lwinmm -lmswsock ^
-DBUILDING_DLL -DENGINE_EDITOR -DPULSE_GRAPHIC_OPENGL -DPULSE_WINDOWS -DIMGUI_IMPL_OPENGL_LOADER_GLAD ^
-o Build/PulseEditor.exe

if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
) else (
    echo Build succeeded!
)
