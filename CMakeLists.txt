cmake_minimum_required(VERSION 4.0.1)
project(PulseEngineEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(ENABLE_ENGINE_EDITOR "Enable Engine Editor features" ON)

include(FetchContent)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Distribution")

set(DOUBLE_PRECISION OFF)
set(GENERATE_DEBUG_SYMBOLS ON)
set(OVERRIDE_CXX_FLAGS ON)
set(CROSS_PLATFORM_DETERMINISTIC OFF)
set(INTERPROCEDURAL_OPTIMIZATION ON)
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
set(CPP_EXCEPTIONS_ENABLED OFF)
set(CPP_RTTI_ENABLED OFF)
set(OBJECT_LAYER_BITS 16)
set(USE_SSE4_1 ON)
set(USE_SSE4_2 ON)
set(USE_AVX ON)
set(USE_AVX2 ON)
set(USE_AVX512 OFF)
set(USE_LZCNT ON)
set(USE_TZCNT ON)
set(USE_F16C ON)
set(USE_FMADD ON)
set(JOLT_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)          # statique
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  # runtime statique
# --- External -------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.4.0"
		SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)


if (MSVC)

	set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "x64")


	if (USE_STATIC_MSVC_RUNTIME_LIBRARY)
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Gm- /MP /nologo /diagnostics:classic /FC /fp:except- /Zc:inline")
endif()

set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTION "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")


# ===========================================================
#  AGGREGATION DE TOUTES LES SOURCES
# ===========================================================

set(PE_ALL_SOURCES
    # --- PE_Registery ---
    src/PulseEngine/core/PulseObject/PulseObject.cpp
    src/PulseEngine/core/GUID/GuidGenerator.cpp

    # --- PE_FileSystem ---
    src/PulseEngine/core/FileManager/FileManager.cpp
    src/PulseEngine/core/FileManager/FileReader/FileReader.cpp

    # --- PL_InputsSystem ---
    src/PulseEngine/core/Input/InputSystem.cpp

    # --- PE_Renderer ---
    src/shader.cpp

    # --- PE_coreBackend ---
    src/glad.c
    src/camera.cpp
    src/PulseEngine/core/PulseEngineBackend.cpp
    src/PulseEngine/core/Inputs/Mouse.cpp
    src/PulseEngine/core/Entity/Entity.cpp
    src/PulseEngine/core/Meshes/mesh.cpp
    src/PulseEngine/core/WindowContext/WindowContext.cpp
    src/PulseEngine/core/Material/Material.cpp
    src/PulseEngine/core/Material/MaterialManager.cpp
    src/PulseEngine/core/Meshes/primitive/Primitive.cpp
    src/PulseEngine/core/GUID/GuidReader.cpp
    src/PulseEngine/core/SceneLoader/SceneLoader.cpp
    src/PulseEngine/core/Lights/DirectionalLight/DirectionalLight.cpp
    src/PulseEngine/CustomScripts/ScriptsLoader.cpp
    src/PulseEngine/core/Physics/Collider/BoxCollider.cpp
    src/PulseEngine/core/Meshes/SkeletalMesh.cpp
    src/PulseEngine/core/Lights/PointLight/PointLight.cpp
    src/PulseEngine/core/Material/Texture.cpp
    src/PulseEngine/core/Lights/LightManager.cpp
    src/PulseEngine/core/Physics/CollisionManager.cpp
    src/PulseEngine/core/coroutine/CoroutineManager.cpp
    src/PulseEngine/ModuleLoader/ModuleLoader.cpp
    src/PulseEngine/API/EngineApi.cpp
    src/PulseEngine/API/GameEntity.cpp
    src/PulseEngine/core/Math/Transform/Transform.cpp
    src/PulseEngine/core/GUID/GuidCollection.cpp
    src/PulseEngine/core/ExecutableManager/ExecutableLauncher.cpp
    src/PulseEngine/core/ExecutableManager/PulseExecutable.cpp
    src/PulseEngine/core/ExecutableManager/ExecutableCommunication.cpp
    src/PulseEngine/API/EntityAPI/EntityApi.cpp
    src/PulseEngine/core/Network/Request/Request.cpp
    PulseEngineEditor/InterfaceEditor/Account/Account.cpp
    src/PulseEngine/API/MaterialAPI/MaterialApi.cpp
    src/PulseEngine/core/Meshes/RenderableMesh.cpp
    src/PulseEngine/core/Meshes/StaticMesh.cpp
    src/PulseEngine/core/Profiler/Profiler.cpp
    src/PulseEngine/core/Profiler/ProfileTimer.cpp
    src/PulseEngine/core/Lights/Lights.cpp
    src/PulseEngine/core/SceneManager/SceneManager.cpp
    src/PulseEngine/core/SceneManager/SpatialPartition/SpatialPartition.cpp
    src/PulseEngine/core/Math/Frustum/Frustum.cpp
    src/PulseEngine/core/SceneManager/SpatialPartition/SimpleSpatial/SimpleSpatial.cpp
    src/PulseEngine/core/Physics/Cast/Casting.cpp
    src/PulseEngine/core/PulseScript/PulseInterpreter.cpp
    src/PulseEngine/core/PulseScript/PulseLexer.cpp
    src/PulseEngine/core/PulseScript/PulseParser.cpp
    src/PulseEngine/core/PulseScript/PulseScript.cpp
    src/PulseEngine/core/PulseScript/PulseScriptsManager.cpp
    src/PulseEngine/core/PulseScript/NativeInit.cpp
    src/PulseEngine/core/Gamemode/Gamemode.cpp
    src/PulseEngine/core/Gamemode/HudController/Widget.cpp
    src/PulseEngine/core/Gamemode/HudController/HudController.cpp
    src/PulseEngine/core/Gamemode/HudController/WidgetComponent/WidgetComponent.cpp
    src/PulseEngine/core/Gamemode/HudController/WidgetComponent/TextComponent/TextComponent.cpp
    src/PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.cpp
    src/PulseEngine/core/Graphics/OpenGLAPI/TextRendererGl.cpp
    src/PulseEngine/core/Graphics/stb_truetype_impl.cpp
    src/glad.c
    src/PulseEngine/API/CameraAPI/CameraAPI.cpp
    src/PulseEngine/API/InputAPI/InputAPI.cpp
    src/PulseEngine/core/Material/ShaderManager.cpp
    src/PulseEngine/core/Physics/PhysicManager.cpp
    src/PulseEngine/core/Physics/PhysicAPI/PhysicAPI.cpp
    src/PulseEngine/core/Physics/PhysicCommand/PhysicsCommand.cpp
)

# --- ImGui + TextEditor conditionnel ---
if(ENABLE_ENGINE_EDITOR)
    list(APPEND PE_ALL_SOURCES
        include/imgui/ImGuizmo.cpp
        include/imgui-node/imgui_node_editor.cpp
        include/imgui-node/imgui_node_editor_api.cpp
        include/imgui-node/imgui_canvas.cpp
        include/imgui-node/crude_json.cpp
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imgui.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp

        src/imGUI/TextEditor/buffer.cpp
        src/imGUI/TextEditor/commands.cpp
        src/imGUI/TextEditor/display.cpp
        src/imGUI/TextEditor/editor.cpp
        src/imGUI/TextEditor/filesystem.cpp
        src/imGUI/TextEditor/glyph_iterator.cpp
        src/imGUI/TextEditor/indexer.cpp
        src/imGUI/TextEditor/keymap.cpp
        src/imGUI/TextEditor/line_widgets.cpp
        src/imGUI/TextEditor/mcommon/animation/timer.cpp
        src/imGUI/TextEditor/mcommon/file/path.cpp
        src/imGUI/TextEditor/mcommon/string/stringutils.cpp
        src/imGUI/TextEditor/mode.cpp
        src/imGUI/TextEditor/mode_search.cpp
        src/imGUI/TextEditor/mode_standard.cpp
        src/imGUI/TextEditor/mode_tree.cpp
        src/imGUI/TextEditor/mode_vim.cpp
        src/imGUI/TextEditor/range_markers.cpp
        src/imGUI/TextEditor/scroller.cpp
        src/imGUI/TextEditor/splits.cpp
        src/imGUI/TextEditor/syntax.cpp
        src/imGUI/TextEditor/syntax_markdown.cpp
        src/imGUI/TextEditor/syntax_providers.cpp
        src/imGUI/TextEditor/syntax_rainbow_brackets.cpp
        src/imGUI/TextEditor/syntax_tree.cpp
        src/imGUI/TextEditor/tab_window.cpp
        src/imGUI/TextEditor/theme.cpp
        src/imGUI/TextEditor/window.cpp

        PulseEngineEditor/InterfaceEditor/InterfaceEditor.cpp
        PulseEngineEditor/InterfaceEditor/TopBar.cpp
        PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.cpp
        PulseEngineEditor/InterfaceEditor/BuildGameCoroutine.cpp
        PulseEngineEditor/InterfaceEditor/InterfaceEditorFileExplorer.cpp
        PulseEngineEditor/InterfaceEditor/Synapse/Synapse.cpp
        PulseEngineEditor/InterfaceEditor/NewFileCreator/NewFileManager.cpp
        PulseEngineEditor/InterfaceEditor/ViewportRenderer.cpp
        PulseEngineEditor/InterfaceEditor/WidgetEditor/WidgetEditor.cpp
        PulseEngineEditor/InterfaceEditor/Console.cpp
        PulseEngineEditor/InterfaceEditor/Tools/EntityEditor/EntityEditor.cpp
        PulseEngineEditor/InterfaceEditor/Tools/MaterialEditor/MaterialEditor.cpp
        PulseEngineEditor/InterfaceEditor/UiHelperTool/UiHelperTool.cpp
    )
endif()

# ===========================================================
#  INCLUDE DIRECTORIES FUSIONNÉS
# ===========================================================
set(PE_ALL_INCLUDES
    include
    src
    /
    include/websocketpp
    include/asio
    include/imgui
    include/imgui-node
    external/assimp/include
    external/assimp/build/include
    src/PulseEngine/core/PulseScript
)

# ===========================================================
#  DLL UNIQUE
# ===========================================================

add_library(PulseEngineAll SHARED ${PE_ALL_SOURCES})

target_include_directories(PulseEngineAll 
PUBLIC ${PE_ALL_INCLUDES}
    ${JoltPhysics_SOURCE_DIR}/..)

target_compile_definitions(PulseEngineAll PRIVATE
    BUILDING_DLL
    PULSE_WINDOWS
    PULSE_GRAPHIC_OPENGL
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    ZEP_FEATURE_CPP_FILE_SYSTEM

)

if(ENABLE_ENGINE_EDITOR)
    target_compile_definitions(PulseEngineAll PRIVATE ENGINE_EDITOR)
endif()

target_link_libraries(PulseEngineAll PRIVATE
    ${CMAKE_SOURCE_DIR}/external/assimp/build/lib/Release/assimp-vc143-mt.lib
    Jolt
)
target_link_libraries(PulseEngineAll PUBLIC glfw)

set_target_properties(PulseEngineAll PROPERTIES
    OUTPUT_NAME "PulseEngine.AllInOne"
)

set(PULSEGAME_SOURCES
    src/main.cpp
)

if(ENABLE_ENGINE_EDITOR)
    list(APPEND PULSEGAME_SOURCES
        src/imGUI/TextEditor/buffer.cpp
        src/imGUI/TextEditor/commands.cpp
        src/imGUI/TextEditor/display.cpp
        src/imGUI/TextEditor/editor.cpp
        src/imGUI/TextEditor/filesystem.cpp
        src/imGUI/TextEditor/glyph_iterator.cpp
        src/imGUI/TextEditor/indexer.cpp
        src/imGUI/TextEditor/keymap.cpp
        src/imGUI/TextEditor/line_widgets.cpp
        src/imGUI/TextEditor/mcommon/animation/timer.cpp
        src/imGUI/TextEditor/mcommon/file/path.cpp
        src/imGUI/TextEditor/mcommon/string/stringutils.cpp
        src/imGUI/TextEditor/mode.cpp
        src/imGUI/TextEditor/mode_search.cpp
        src/imGUI/TextEditor/mode_standard.cpp
        src/imGUI/TextEditor/mode_tree.cpp
        src/imGUI/TextEditor/mode_vim.cpp
        src/imGUI/TextEditor/range_markers.cpp
        src/imGUI/TextEditor/scroller.cpp
        src/imGUI/TextEditor/splits.cpp
        src/imGUI/TextEditor/syntax.cpp
        src/imGUI/TextEditor/syntax_markdown.cpp
        src/imGUI/TextEditor/syntax_providers.cpp
        src/imGUI/TextEditor/syntax_rainbow_brackets.cpp
        src/imGUI/TextEditor/syntax_tree.cpp
        src/imGUI/TextEditor/tab_window.cpp
        src/imGUI/TextEditor/theme.cpp
        src/imGUI/TextEditor/window.cpp
    )
endif()

add_executable(PulseGame ${PULSEGAME_SOURCES})

target_compile_definitions(PulseGame PRIVATE
    PULSE_WINDOWS
    PULSE_GRAPHIC_OPENGL
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    ZEP_FEATURE_CPP_FILE_SYSTEM

)

if(ENABLE_ENGINE_EDITOR)
    target_compile_definitions(PulseGame PRIVATE ENGINE_EDITOR)
endif()


target_link_libraries(PulseGame PRIVATE PulseEngineAll Jolt)

target_include_directories(PulseGame PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/imgui-node
    ${CMAKE_SOURCE_DIR}/include/imgui/backends
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/.
    ${JoltPhysics_SOURCE_DIR}/..

)