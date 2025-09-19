cmake_minimum_required(VERSION 3.10)
project(PulseEngineEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# =====================
# Compiler definitions
# =====================
add_definitions(
    -DBUILDING_DLL
    -DENGINE_EDITOR
    -DPULSE_GRAPHIC_OPENGL
    -DPULSE_WINDOWS
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
)

# =====================
# Include directories
# =====================
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-node
    ${CMAKE_SOURCE_DIR}/src
    E:/Editor/Include
    external/assimp/include
)

# =====================
# Source files
# =====================
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/PulseEngine/**/*.cpp
    PulseEngineEditor/InterfaceEditor/**/*.cpp
    imgui/*.cpp
    imgui/backends/*.cpp
    imgui-node/*.cpp
)

# =====================
# Build the shared library
# =====================
add_library(PulseEngineEditor SHARED ${ENGINE_SOURCES})
include_directories(DLLS)
# =====================
# Link external libraries
# =====================
target_link_libraries(PulseEngineEditor
    glfw3
    opengl32
    m
    assimp
    comdlg32
)

# =====================
# Link directories
# =====================
link_directories(
    external/assimp/lib
    external/assimp/lib/x64
    D:/MSYS2/mingw64/lib
)

# =====================
# Post-build steps
# =====================
# Copy DLL and .a to distribuables
add_custom_command(TARGET PulseEngineEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/distribuables
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:PulseEngineEditor>
        ${CMAKE_SOURCE_DIR}/distribuables/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_LINKER_FILE:PulseEngineEditor>
        ${CMAKE_SOURCE_DIR}/distribuables/
)

# Copy source & include folders for distribution (optional)
add_custom_command(TARGET PulseEngineEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/src
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/src
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/include
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/src/*.cpp
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/PulseEngineBuild/dist/include/*.cpp
)
