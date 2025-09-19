/**
 * @file PULSE_ENGINE_DLL_API.h
 * @brief Defines the PULSE_ENGINE_DLL_API macro for DLL export/import on different platforms.
 *
 * This header allows you to export and import symbols from a DLL/shared library
 * in a platform-agnostic way.
 *
 * - On Windows, it uses `__declspec(dllexport)` and `__declspec(dllimport)`.
 * - On other platforms (Linux, macOS), it falls back to an empty definition.
 *
 * To use:
 * - Define `BUILDING_DLL` when building the DLL.
 * - Include this header and use `PULSE_ENGINE_DLL_API` before class/function declarations.
 *
 * @note This is a common pattern for cross-platform C++ shared libraries.
 */

#pragma once

// ----------------------------------------------------------------------------
// DLL export/import definition
// ----------------------------------------------------------------------------
#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef BUILDING_DLL
    /// Export symbols when building the DLL on Windows.
    #define PULSE_ENGINE_DLL_API __declspec(dllexport)
  #else
    /// Import symbols when using the DLL on Windows.
    #define PULSE_ENGINE_DLL_API __declspec(dllimport)
  #endif
#else
  /// No-op for non-Windows platforms (Linux/macOS).
  #define PULSE_ENGINE_DLL_API
#endif
