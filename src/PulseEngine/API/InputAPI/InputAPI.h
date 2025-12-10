#ifndef __INPUTAPI_H__
#define __INPUTAPI_H__

#include "Common/dllExport.h"

/**
 * @class InputAPI
 * @brief Static interface providing real-time access to input state.
 *
 * InputAPI exposes frame-accurate keyboard, mouse and action-based queries
 * intended for gameplay logic, UI systems, character controllers, and
 * real-time decision behaviors.  
 *
 * All functions operate on the input state accumulated during the current
 * engine frame.  
 *  
 * Action identifiers are engine-defined and mapped to keys, mouse buttons,
 * or other devices through the project's input configuration.
 */
class PULSE_ENGINE_DLL_API InputAPI
{
public:

    /**
     * @brief Returns whether the given action is currently held down.
     *
     * This reflects continuous input state (e.g., a key or button held).
     *
     * @param actionId Engine-assigned action identifier.
     * @return True if the action is currently down, false otherwise.
     */
    static bool isActionDown(int actionId);

    /**
     * @brief Returns true only on the first frame an action is pressed.
     *
     * Commonly used for triggers, toggles, or events that should fire
     * once per press (e.g., jumping, firing a weapon, confirming UI).
     *
     * @param actionId Engine-assigned action identifier.
     * @return True if the action transitioned from "up" to "down" this frame.
     */
    static bool wasActionPressed(int actionId);

    /**
     * @brief Returns true only on the frame an action is released.
     *
     * Useful for detecting button release events, charge mechanics,
     * or UI interactions requiring release confirmation.
     *
     * @param actionId Engine-assigned action identifier.
     * @return True if the action transitioned from "down" to "up" this frame.
     */
    static bool wasActionReleased(int actionId);

    /**
     * @brief Retrieves the current mouse X position relative to the window.
     *
     * The value is expressed in pixels in screen space.
     *
     * @return Horizontal mouse coordinate.
     */
    static double getMouseX();

    /**
     * @brief Retrieves the current mouse Y position relative to the window.
     *
     * The value is expressed in pixels in screen space.
     *
     * @return Vertical mouse coordinate.
     */
    static double getMouseY();
};

#endif // __INPUTAPI_H__
