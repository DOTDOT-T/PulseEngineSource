/**
 * @file EditorDefines.h
 * @brief Defines macros and helper functions used specifically in editor or game builds.
 *
 * This header provides a mechanism for conditionally including/excluding code
 * based on whether the engine is built in **editor** or **runtime** (game) mode.
 *
 * Use the following macros:
 * - `EDITOR_ONLY(code)`: Includes `code` only in editor builds.
 * - `IN_GAME_ONLY(code)`: Includes `code` only in game builds.
 * - `EDITOR_LOG(msg)`: Logs editor messages with timestamps to `std::cout`.
 * - `EDITOR_ERROR(msg)`: Logs error messages with timestamps to `std::cerr`.
 *
 * @note Define `ENGINE_EDITOR` at compile time to enable editor features.
 */

#ifndef EDITORDEFINES_H
#define EDITORDEFINES_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>


    /**
     * @brief Returns the current date and time as a formatted string.
     *
     * Used for timestamping editor logs.
     *
     * @return A string in the format "YYYY-MM-DD HH:MM:SS".
     */
    inline std::string GetCurrentDateTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm* nowTm = std::localtime(&nowTime);

        std::ostringstream oss;
        oss << std::put_time(nowTm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
// ----------------------------------------------------------------------------
// Editor Mode Definitions
// ----------------------------------------------------------------------------
#ifdef ENGINE_EDITOR


#include "PulseEngineEditor/InterfaceEditor/Console.h"

    /// Includes `code` only when compiling in editor mode.
    #define EDITOR_ONLY(code) code

    /// Strips `code` from editor builds; used to isolate game/runtime-only code.
    #define IN_GAME_ONLY(code)


    #define EDITOR_LOG(msg)                                                         \
        do {                                                                        \
            std::ostringstream oss;                                                 \
            oss << "[" << GetCurrentDateTime() << "] [LOG] ["                       \
                << __FUNCTION__ << "]: " << msg << std::endl;                       \
            Console::Push(oss.str());                                               \
            std::cout << oss.str() << std::endl;                                    \
        } while(0);



    #define EDITOR_INFO(msg)                                                        \
        do {                                                                        \
            std::ostringstream oss;                                                 \
            oss << "[" << GetCurrentDateTime() << "] [INFO] ["                      \
                << __FUNCTION__ << "]: " << msg << std::endl;                       \
            Console::Push(oss.str());                                 \
            std::cout << oss.str() << std::endl;                                    \
        } while(0);

#define EDITOR_WARN(msg)                                                            \
    do {                                                                            \
        std::ostringstream oss;                                                     \
        oss << "[" << GetCurrentDateTime() << "] [WARN] ["                          \
            << __FUNCTION__ << "]: " << msg << std::endl;                           \
        Console::Push(oss.str());                                     \
            std::cout << oss.str() << std::endl;                                    \
    } while(0);


#define EDITOR_ERROR(msg)                                                           \
    do {                                                                            \
        std::ostringstream oss;                                                     \
        oss << "[" << GetCurrentDateTime() << "] [ERROR] ["                         \
            << __FUNCTION__ << "]: " << msg << std::endl;                           \
        Console::Push(oss.str());                                     \
            std::cout << oss.str() << std::endl;                                    \
    } while(0);
#define EDITOR_SUCCESS(msg)                                                           \
    do {                                                                            \
        std::ostringstream oss;                                                     \
        oss << "[" << GetCurrentDateTime() << "] [SUCCESS] ["                         \
            << __FUNCTION__ << "]: " << msg << std::endl;                           \
        Console::Push(oss.str());                                     \
            std::cout << oss.str() << std::endl;                                    \
    } while(0);


#else

    // ----------------------------------------------------------------------------
    // Game Mode (Runtime Build) - Editor features are excluded
    // ----------------------------------------------------------------------------

    /// Strips `code` from game builds; used to isolate editor-only logic.
    #define EDITOR_ONLY(code)

    /// Includes `code` only when compiling in game mode.
    #define IN_GAME_ONLY(code) code

    /// Logs a message to standard output with a timestamp, editor tag, and function name.
    #define EDITOR_LOG(msg) \
        std::cout << "[" << GetCurrentDateTime() << "] [LOG] [" << __FUNCTION__ << "]: " << msg << std::endl;
    
    /// Logs a message to standard output with a timestamp, editor tag, and function name.
    #define EDITOR_INFO(msg) \
        std::cout << "[" << GetCurrentDateTime() << "] [INFO] [" << __FUNCTION__ << "]: " << msg << std::endl;

    #define EDITOR_WARN(msg) \
        std::cout << "[" << GetCurrentDateTime() << "] [WARN] [" << __FUNCTION__ << "]: " << msg << std::endl;


    /// Logs an error message to standard error with a timestamp, editor tag, and function name.
    #define EDITOR_ERROR(msg) \
        std::cerr << "[" << GetCurrentDateTime() << "] [ERROR] [" << __FUNCTION__ << "]: " << msg << std::endl;

#endif // ENGINE_EDITOR

#ifdef PULSE_PROFILING 
    #define PROFILE_TIMER_SCOPE(name) ProfileTimer profileTimer##__LINE__(name)
    #define PROFILE_TIMER_FUNCTION PROFILE_TIMER_SCOPE(__func__)
#else
    #define PROFILE_TIMER_SCOPE(name) 
    #define PROFILE_TIMER_FUNCTION 
#endif


// --------------------
// Asset Path Configuration
// --------------------
#ifdef ENGINE_EDITOR
    #define ASSET_PATH "PulseEngineEditor/"  // Editor-specific asset path
#else
    #define ASSET_PATH "PulseEngineEditor/"            // Runtime asset path
#endif

#define PulseEngineInstance PulseEngineBackend::GetInstance()
#define PulseEngineGraphicsAPI PulseEngineBackend::graphicsAPI
#define PulseEngineDeltaTime PulseEngineBackend::GetDeltaTime()

#endif // EDITORDEFINES_H
