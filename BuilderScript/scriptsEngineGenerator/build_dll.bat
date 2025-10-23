@echo off
setlocal enabledelayedexpansion
echo === Compilation of Pulse Engine.dll for Window ===

set OBJFILES=
for %%f in (ObjectFiles\*.o) do (
    set OBJFILES=!OBJFILES! %%f
)

if not exist Build (
    mkdir Build
)
if not exist Build\Modules (
    mkdir Modules
)

if not exist Build\Modules\Interface (
    mkdir Interface
)

g++ -shared -Wl,--out-implib,Build/libPulseEngineEditor.a ^
-o Build/PulseEngineEditor.dll ^
%OBJFILES% ^
src/glad.c ^
src/PulseEngine/core/GUID/GuidGenerator.cpp ^
src/shader.cpp ^
src/camera.cpp ^
src/PulseEngine/core/PulseEngineBackend.cpp ^
src/PulseEngine/core/Inputs/Mouse.cpp ^
src/PulseEngine/core/Entity/Entity.cpp ^
src/PulseEngine/core/Meshes/mesh.cpp ^
src/PulseEngine/core/WindowContext/WindowContext.cpp ^
src/PulseEngine/core/Material/Material.cpp ^
src/PulseEngine/core/Material/MaterialManager.cpp ^
src/PulseEngine/core/Meshes/primitive/Primitive.cpp ^
src/PulseEngine/core/GUID/GuidReader.cpp ^
src/PulseEngine/core/SceneLoader/SceneLoader.cpp ^
src/PulseEngine/core/FileManager/FileManager.cpp ^
src/PulseEngine/core/Lights/DirectionalLight/DirectionalLight.cpp ^
src/PulseEngine/CustomScripts/ScriptsLoader.cpp ^
src/PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.cpp ^
src/PulseEngine/core/Physics/Collider/BoxCollider.cpp ^
src/PulseEngine/core/Meshes/SkeletalMesh.cpp ^
PulseEngineEditor/InterfaceEditor/InterfaceEditor.cpp ^
PulseEngineEditor/InterfaceEditor/TopBar.cpp ^
PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.cpp ^
src/PulseEngine/core/Lights/PointLight/PointLight.cpp ^
src/PulseEngine/core/Material/Texture.cpp ^
src/PulseEngine/core/Lights/LightManager.cpp ^
src/PulseEngine/core/Physics/CollisionManager.cpp ^
src/PulseEngine/core/coroutine/CoroutineManager.cpp ^
PulseEngineEditor/InterfaceEditor/BuildGameCoroutine.cpp ^
src/PulseEngine/ModuleLoader/ModuleLoader.cpp ^
src/PulseEngine/API/EngineApi.cpp ^
src/PulseEngine/API/GameEntity.cpp ^
src/PulseEngine/core/Math/Transform/Transform.cpp ^
PulseEngineEditor\InterfaceEditor\InterfaceEditorFileExplorer.cpp ^
PulseEngineEditor\InterfaceEditor\Synapse\Synapse.cpp ^
PulseEngineEditor\InterfaceEditor\NewFileCreator\NewFileManager.cpp ^
src\PulseEngine\core\GUID\GuidCollection.cpp ^
src\PulseEngine\core\ExecutableManager\ExecutableLauncher.cpp ^
src\PulseEngine\core\ExecutableManager\PulseExecutable.cpp ^
src\PulseEngine\core\ExecutableManager\ExecutableCommunication.cpp ^
src\PulseEngine\API\EntityAPI\EntityApi.cpp ^
include/imgui/backends/imgui_impl_glfw.cpp ^
include/imgui/backends/imgui_impl_opengl3.cpp ^
include\imgui\ImGuizmo.cpp ^
include/imgui-node/imgui_node_editor.cpp ^
include/imgui-node/imgui_node_editor_api.cpp ^
include/imgui-node/imgui_canvas.cpp ^
include/imgui-node/crude_json.cpp ^
src\PulseEngine\core\Network\Request\Request.cpp ^
src\PulseEngine\core\FileManager\FileReader\FileReader.cpp ^
PulseEngineEditor/InterfaceEditor/Account/Account.cpp ^
src/PulseEngine/core/Input/InputSystem.cpp ^
src\PulseEngine\API\MaterialAPI\MaterialApi.cpp ^
src\PulseEngine\core\Meshes\RenderableMesh.cpp ^
src\PulseEngine\core\Meshes\StaticMesh.cpp ^
src\PulseEngine\core\Profiler\Profiler.cpp ^
src\PulseEngine\core\Profiler\ProfileTimer.cpp ^
src\PulseEngine\core\PulseObject\PulseObject.cpp ^
src\PulseEngine\core\Lights\Lights.cpp ^
src\PulseEngine\core\SceneManager\SceneManager.cpp ^
-I. -Iinclude -Iinclude/websocketpp -Iinclude/asio -Iinclude/imgui -Iinclude/imgui-node -I./src -IE:/Editor/Include -Iexternal/assimp/include ^
-Lexternal/assimp/lib ^
-Lexternal/assimp/lib/x64 ^
-LD:/MSYS2/mingw64/lib ^
-lglfw3 -lopengl32 -lm -lassimp -lcomdlg32 -lws2_32 -lwinmm -lmswsock ^
-DBUILDING_DLL -DENGINE_EDITOR -DPULSE_GRAPHIC_OPENGL -DPULSE_WINDOWS -DIMGUI_IMPL_OPENGL_LOADER_GLAD

if %errorlevel% neq 0 (
    echo DLL build failed!
    exit /b %errorlevel%
) else (
    echo DLL build succeeded!
    if not exist distribuables (
        mkdir distribuables
    )
    copy /Y Build\libPulseEngineEditor.a distribuables\
    copy /Y Build\PulseEngineEditor.dll distribuables\
    
    if not exist Build\dist\src (
        mkdir Build\dist\src
    )
    if not exist Build\dist\include (
        mkdir Build\dist\include
    )
    xcopy /E /I /Y src Build\dist\src
    copy /Y src\main.cpp Build\dist\main.cpp
    xcopy /E /I /Y include Build\dist\include
    
    del /S /Q Build\dist\src\*.cpp
    del /S /Q Build\dist\include\*.cpp


    if not exist ..\ModuleCreator\src (
        mkdir ..\ModuleCreator\src
    )
    if not exist ..\ModuleCreator\include (
        mkdir ..\ModuleCreator\include
    )

    if not exist ..\ModuleCreator\PulseEngineEditor (
        mkdir ..\ModuleCreator\PulseEngineEditor
    )

    copy /Y Build\libPulseEngineEditor.a ..\ModuleCreator\lib

    xcopy /E /I /Y src ..\ModuleCreator\src
    xcopy /E /I /Y include ..\ModuleCreator\include
    xcopy /E /I /Y PulseEngineEditor ..\ModuleCreator\PulseEngineEditor

    del /S /Q ..\ModuleCreator\src\*.cpp
    del /S /Q ..\ModuleCreator\include\*.cpp
    del /S /Q ..\ModuleCreator\PulseEngineEditor\*.cpp

)

