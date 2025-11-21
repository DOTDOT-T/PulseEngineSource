/**
 * @file common.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Common.h - Shared includes, types, and configuration flags for the engine
 * @brief This header centralizes commonly used includes and definitions across
 * @brief the engine and editor builds. It helps to avoid redundancy and ensures
 * @brief consistent access to shared utilities (e.g., math types, rendering APIs).
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef COMMON_H
#define COMMON_H

// --------------------
// Standard Library Includes
// --------------------
#include <cstddef>      // for std::size_t
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// --------------------
// Engine Math Utilities
// --------------------
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Mat3.h"
#include "PulseEngine/core/Math/Quaternion.h"
#include "PulseEngine/core/Math/Color.h"
#include "PulseEngine/core/Math/Transform/Transform.h"

// --------------------
// Engine Core Includes
// --------------------
#include "shader.h"
#include "camera.h"
#include "PulseEngine/core/Material/Texture.h"
#include "PulseEngine/ModuleLoader/ModuleLoader.h"
#include "PulseEngine/ModuleLoader/IModule/IModule.h"
#include "PulseEngine/core/FileManager/FileManager.h"

#define ASIO_STANDALONE

// --------------------
// Platform-Specific Includes
// --------------------
// -----------------------------
// Windows
// -----------------------------
#ifdef PULSE_WINDOWS
    #include "winsock2.h"
    #include <Windows.h>      // Core Windows API
    #include <direct.h>       // _mkdir, _getcwd
#endif

// -----------------------------
// Linux
// -----------------------------
#ifdef PULSE_LINUX
#endif

// -----------------------------
// macOS
// -----------------------------
#ifdef PULSE_MACOS
#endif

// -----------------------------
// Android
// -----------------------------
#ifdef PULSE_ANDROID
#endif

// -----------------------------
// iOS
// -----------------------------
#ifdef PULSE_IOS
#endif

// -----------------------------
// Web (Emscripten)
// -----------------------------
#ifdef PULSE_WEB
#endif

// -----------------------------
// PlayStation 4
// -----------------------------
#ifdef PULSE_PLAYSTATION4
#endif

// -----------------------------
// PlayStation 5
// -----------------------------
#ifdef PULSE_PLAYSTATION5
#endif

// -----------------------------
// Xbox One
// -----------------------------
#ifdef PULSE_XBOXONE
#endif

// -----------------------------
// Nintendo Switch 2
// -----------------------------
#ifdef PULSE_SWITCH2
#endif
    

// --------------------
// Editor/Build Mode Flags
// --------------------
#include "Common/EditorDefines.h"

// --------------------
// Graphics Includes (only in engine or editor build with OpenGL)
// --------------------
#ifdef PULSE_GRAPHIC_OPENGL
    #include <glad.h>                       // OpenGL function loader
    #include <GLFW/glfw3.h>                 // Cross-platform windowing/input
    #include <glm/glm.hpp>                  // GLM core
    #include <glm/gtc/type_ptr.hpp>         // GLM pointer helpers
    #include <glm/gtc/matrix_transform.hpp> // GLM transformation helpers
    #include <GL/gl.h>                      // OpenGL headers
#endif

// --------------------
// Editor UI Integration (ImGui)
// --------------------
#ifdef ENGINE_EDITOR
//     #define IMGUI_DEFINE_MATH_OPERATORS
//     #include "imgui.h"
//     #include "imgui_internal.h"
//     #include "backends/imgui_impl_glfw.h"
//     #include "backends/imgui_impl_opengl3.h"
//     #include "imgui-node/imgui_node_editor.h"
//     #include "imgui-node/imgui_node_editor_internal.h"
// namespace ed = ax::NodeEditor;

#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#define ASIO_STANDALONE


#include "PulseEngine/core/WindowContext/WindowContext.h"
#include "PulseEngine/core/Profiler/Profiler.h"
#include "PulseEngine/core/Profiler/TraceEvent.h"
#include "PulseEngine/core/Profiler/ProfileTimer.h"



// --------------------
// Asset Path Configuration
// --------------------
#ifdef ENGINE_EDITOR
    #define ASSET_PATH "PulseEngineEditor/"  // Editor-specific asset path
#else
    #define ASSET_PATH "PulseEngineEditor/"             // Runtime asset path
#endif

// --------------------
// Engine Types
// --------------------
// Globally unique identifier type used across the engine
using GUID_PULSE = std::size_t;

#include <filesystem>
namespace fs = std::filesystem;
struct ClickedFileData
{
    fs::path path;
    fs::path name;
};


#endif // COMMON_H
