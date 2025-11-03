/**
 * @file InputSystem.h
 * @author Dorian LEXTERIAQUE
 * @brief Cross-platform input management system for the Pulse Engine.
 * 
 * @details
 * The InputSystem class abstracts keyboard and mouse input across different
 * platforms (currently Windows, extendable to Linux/macOS). It provides a
 * high-level action-binding API, allowing the engine and editor to respond
 * to user input without dealing with low-level platform-specific details.
 *
 * Responsibilities:
 * - Poll and store current and previous key/mouse states.
 * - Provide an action-to-input mapping system for flexible control schemes.
 * - Support querying key and mouse states (Down, Pressed, Released, Up).
 * - Persist input bindings for editor and runtime use.
 * 
 * @section Usage Example
 * @code
 *  InputSystem input;
 *  input.bindAction(0, 'W');               // Move forward
 *  input.bindMouseAction(1, LEFT_MOUSE_BUTTON); // Shoot
 *  
 *  input.newFrame();                        // Update state each frame
 *  
 *  if (input.isActionDown(0)) { ... }      // Respond to input
 *  if (input.wasActionPressed(1)) { ... }
 * @endcode
 * 
 * @note Mouse coordinates are stored as doubles (pixels) and are updated
 *       every frame by pollMouse().
 * @warning Not thread-safe. All queries and bindings must be performed
 *          on the main thread.
 * 
 * @version 0.1
 * @date 2025-09-29
 * @see OneBinding
 * @see KeyState
 */

#pragma once

// #include "pch.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"

#include <windows.h>
#include "Common/dllExport.h"

/// Left mouse button index
#define LEFT_MOUSE_BUTTON 0
/// Right mouse button index
#define RIGHT_MOUSE_BUTTON 1

namespace PulseLibs
{

    /**
     * @struct OneBinding
     * @brief Represents a single action-to-input mapping.
     *
     * Contains the action ID and the corresponding key or mouse button.
     */
    struct OneBinding {
        int actionId; ///< Identifier for the action
        int button;   ///< Key or mouse button code
    
        OneBinding() : actionId(-1), button(-1) {}
        OneBinding(int actionId, int button) : actionId(actionId), button(button) {}
    };
    
    /**
     * @class InputSystem
     * @brief Manages keyboard and mouse input and provides an action-binding layer.
     *
     * @details
     * The InputSystem maintains the current and previous state of keys and mouse buttons.
     * It provides functions to bind actions to inputs, query input states, and persist
     * bindings to disk.
     */
    class PULSE_ENGINE_DLL_API InputSystem {
    public:
        /// Represents the state of a key or button
        enum class KeyState {
            Up,       ///< Not pressed
            Down,     ///< Held down
            Pressed,  ///< Pressed this frame
            Released  ///< Released this frame
        };
    
        InputSystem();
        ~InputSystem();
    
        // ========================================================================
        // Frame Update
        // ========================================================================
    
        /// Call at the beginning of each frame to update input states
        void newFrame();
    
        // ========================================================================
        // Binding API
        // ========================================================================
    
        /**
         * @brief Bind a keyboard key to an action ID.
         * @param actionId The action identifier.
         * @param key The key code to bind.
         */
        void bindAction(int actionId, int key);
    
        /**
         * @brief Bind a mouse button to an action ID.
         * @param actionId The action identifier.
         * @param button The mouse button index to bind.
         */
        void bindMouseAction(int actionId, int button);
    
        // ========================================================================
        // Query API
        // ========================================================================
    
        bool isActionDown(int actionId) const;       ///< Returns true if the action is held down
        bool wasActionPressed(int actionId) const;  ///< Returns true if the action was pressed this frame
        bool wasActionReleased(int actionId) const; ///< Returns true if the action was released this frame
    
        double getMouseX() const; ///< Current mouse X position (pixels)
        double getMouseY() const; ///< Current mouse Y position (pixels)
    
        OneBinding GetOneBinding(int actionId) const;                 ///< Retrieve the binding for a given action
        void ChangeBinding(OneBinding newBinding, int actionId);      ///< Update the binding for a given action
    
        /// Saves all current action bindings to disk
        void SaveBindingsToFile();
    
    private:
        // ========================================================================
        // Internal Input Structures
        // ========================================================================
    
        struct KeyInfo {
            int keyCode = -1;       ///< Key code
            KeyState state = KeyState::Up; ///< Current key state
        };
    
        struct MouseInfo {
            int button = -1;        ///< Mouse button index
            KeyState state = KeyState::Up; ///< Current button state
        };
    
        static const int MAX_KEYS = 512;   ///< Maximum number of keyboard keys tracked
        static const int MAX_MOUSE = 8;    ///< Maximum number of mouse buttons tracked
    
        KeyInfo keys[MAX_KEYS];            ///< Current keyboard states
        MouseInfo mouseButtons[MAX_MOUSE]; ///< Current mouse button states
    
        double mouseX; ///< Current mouse X position
        double mouseY; ///< Current mouse Y position
    
        int actionToKey[MAX_KEYS];    ///< Maps actions to keyboard keys
        int actionToMouse[MAX_MOUSE]; ///< Maps actions to mouse buttons
    
        // ========================================================================
        // Internal Polling Methods
        // ========================================================================
    
        /// Polls the keyboard and updates the KeyInfo array
        void pollKeyboard();
    
        /// Polls the mouse and updates MouseInfo array and mouseX/mouseY
        void pollMouse();
    };

}