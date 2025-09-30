#include "InputSystem.h"
#include <iostream>

InputSystem::InputSystem() : mouseX(0.0), mouseY(0.0) {
    for (int i = 0; i < MAX_KEYS; i++) {
        keys[i].keyCode = -1;
        keys[i].state = KeyState::Up;
        actionToKey[i] = -1;
    }
    for (int i = 0; i < MAX_MOUSE; i++) {
        mouseButtons[i].button = -1;
        mouseButtons[i].state = KeyState::Up;
        actionToMouse[i] = -1;
    }

    // read bindings from config file
    FileReader reader("EngineConfig/inputConfig.pconfig");    
    if(!reader.IsOpen()) EDITOR_LOG("No inputs config file found");     
    
    nlohmann::json js = reader.ToJson();
    for(auto& el : js["keyBindings"].items()) 
    {
        int actionId = std::stoi(el.key());
        int key = el.value();
        bindAction(actionId, key);

        EDITOR_LOG("Bound action\t" + std::to_string(actionId) + "\tto key\t" + (char)key);
    }
    

    EDITOR_LOG("Input System load successfully")
}

InputSystem::~InputSystem() {}

void InputSystem::newFrame() {
    // update keys
    for (int i = 0; i < MAX_KEYS; i++) {
        if (keys[i].state == KeyState::Pressed) keys[i].state = KeyState::Down;
        else if (keys[i].state == KeyState::Released) keys[i].state = KeyState::Up;
    }
    // update mouse
    for (int i = 0; i < MAX_MOUSE; i++) {
        if (mouseButtons[i].state == KeyState::Pressed) mouseButtons[i].state = KeyState::Down;
        else if (mouseButtons[i].state == KeyState::Released) mouseButtons[i].state = KeyState::Up;
    }

    pollKeyboard();
    pollMouse();
}

void InputSystem::bindAction(int actionId, int key) {
    if (actionId < 0 || actionId >= MAX_KEYS) return;
    actionToKey[actionId] = key;
    keys[actionId].keyCode = key;
    keys[actionId].state = KeyState::Up;
}

void InputSystem::bindMouseAction(int actionId, int button) {
    if (actionId < 0 || actionId >= MAX_MOUSE) return;
    actionToMouse[actionId] = button;
    mouseButtons[actionId].button = button;
    mouseButtons[actionId].state = KeyState::Up;
}

bool InputSystem::isActionDown(int actionId) const {
    if (actionId < 0 || actionId >= MAX_KEYS) return false;
    KeyState st = keys[actionId].state;
    return st == KeyState::Down || st == KeyState::Pressed;
}

bool InputSystem::wasActionPressed(int actionId) const {
    if (actionId < 0 || actionId >= MAX_KEYS) return false;
    return keys[actionId].state == KeyState::Pressed;
}

bool InputSystem::wasActionReleased(int actionId) const {
    if (actionId < 0 || actionId >= MAX_KEYS) return false;
    return keys[actionId].state == KeyState::Released;
}

double InputSystem::getMouseX() const { return mouseX; }
double InputSystem::getMouseY() const { return mouseY; }

OneBinding InputSystem::GetOneBinding(int actionId) const
{
    return OneBinding(keys[actionId > MAX_KEYS ? MAX_KEYS -1 : actionId].keyCode, mouseButtons[actionId > MAX_MOUSE ? MAX_MOUSE - 1 : actionId].button);
}

void InputSystem::ChangeBinding(OneBinding newBinding, int actionId)
{
    if(actionId < 0 || actionId >= MAX_KEYS) return;
    keys[actionId].keyCode = newBinding.actionId;
    mouseButtons[actionId].button = newBinding.button;
    SaveBindingsToFile();
}

void InputSystem::SaveBindingsToFile()
{
    nlohmann::json js;
    nlohmann::json keyBindings;
    for(int i = 0; i < MAX_KEYS; i++)
    {
        if(keys[i].keyCode != -1)
            keyBindings[std::to_string(i)] = keys[i].keyCode;
    }

    js["keyBindings"] = keyBindings;

    FileReader fileReader("EngineConfig/inputConfig.pconfig");
    if(!fileReader.IsOpen()) EDITOR_LOG("Failed to save input config file");

    fileReader.SaveJson(js);
    fileReader.Close();
}

void InputSystem::pollKeyboard() 
{
    for (int i = 0; i < MAX_KEYS; i++) 
    {
        int vk = keys[i].keyCode;
        if (vk < 0) continue;

        short state = GetAsyncKeyState(vk);
        bool down = (state & 0x8000) != 0;

        if (down) 
        {
            if (keys[i].state == KeyState::Up || keys[i].state == KeyState::Released)
                keys[i].state = KeyState::Pressed;
        } else 
        {
            if (keys[i].state == KeyState::Down || keys[i].state == KeyState::Pressed)
                keys[i].state = KeyState::Released;
        }
    }
}

void InputSystem::pollMouse() {
    POINT p;
    GetCursorPos(&p);
    mouseX = static_cast<double>(p.x);
    mouseY = static_cast<double>(p.y);

    for (int i = 0; i < MAX_MOUSE; i++) {
        int btn = mouseButtons[i].button;
        if (btn < 0) continue;

        bool down = false;
        if (btn == LEFT_MOUSE_BUTTON) down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        else if (btn == RIGHT_MOUSE_BUTTON) down = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

        if (down) {
            if (mouseButtons[i].state == KeyState::Up || mouseButtons[i].state == KeyState::Released)
                mouseButtons[i].state = KeyState::Pressed;
        } else {
            if (mouseButtons[i].state == KeyState::Down || mouseButtons[i].state == KeyState::Pressed)
                mouseButtons[i].state = KeyState::Released;
        }
    }
}
