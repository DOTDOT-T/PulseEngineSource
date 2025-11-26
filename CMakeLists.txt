cmake_minimum_required(VERSION 3.21)
project(PulseEngineEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Download GLFW automatically
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.10  # specify the version you want
)
FetchContent_MakeAvailable(glfw)

# Define the library
add_library(PE_Registery SHARED
    src/PulseEngine/core/PulseObject/PulseObject.cpp
    src/PulseEngine/core/GUID/GuidGenerator.cpp
)

target_compile_definitions(PE_Registery PRIVATE
    BUILDING_DLL
    PULSE_WINDOWS
    PULSE_GRAPHIC_OPENGL
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    ENGINE_EDITOR
)

# Set include directories
target_include_directories(PE_Registery PUBLIC
    include
    src
)

# Link GLFW
target_link_libraries(PE_Registery PUBLIC glfw)

# Set the target name for the DLL explicitly
set_target_properties(PE_Registery PROPERTIES
    OUTPUT_NAME "PulseEngine.Registery"
)
