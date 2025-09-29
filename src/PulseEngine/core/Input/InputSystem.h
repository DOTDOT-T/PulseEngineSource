/**
 * @file InputSystem.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief simple class to manage inputs for different platform
 * @version 0.1
 * @date 2025-09-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "pch.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"

#include <windows.h>
#include "Common/dllExport.h"

#define LEFT_MOUSE_BUTTON 0
#define RIGHT_MOUSE_BUTTON 1

struct OneBinding {
    int actionId;
    int button;

    OneBinding() : actionId(-1), button(-1) {}
    OneBinding(int actionId, int button) : actionId(actionId), button(button) {}
};

class PULSE_ENGINE_DLL_API InputSystem {
public:
    enum class KeyState {
        Up,
        Down,
        Pressed,
        Released
    };

    InputSystem();
    ~InputSystem();

    // Call at the beginning of each frame
    void newFrame();

    // Mapping API
    void bindAction(int actionId, int key);
    void bindMouseAction(int actionId, int button);

    // Query API
    bool isActionDown(int actionId) const;
    bool wasActionPressed(int actionId) const;
    bool wasActionReleased(int actionId) const;

    double getMouseX() const;
    double getMouseY() const;

    OneBinding GetOneBinding(int actionId) const;

    void SaveBindingsToFile();

private:
    struct KeyInfo {
        int keyCode = -1;
        KeyState state = KeyState::Up;
    };

    struct MouseInfo {
        int button = -1;
        KeyState state = KeyState::Up;
    };

    static const int MAX_KEYS = 512;
    static const int MAX_MOUSE = 8;

    KeyInfo keys[MAX_KEYS];
    MouseInfo mouseButtons[MAX_MOUSE];

    double mouseX;
    double mouseY;

    int actionToKey[MAX_KEYS];
    int actionToMouse[MAX_MOUSE];

    void pollKeyboard();
    void pollMouse();
};
