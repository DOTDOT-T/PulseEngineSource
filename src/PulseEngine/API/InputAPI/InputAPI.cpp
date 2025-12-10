#include "InputAPI.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/core/Input/InputSystem.h"

bool InputAPI::isActionDown(int actionId) 
{
    return PulseEngineInstance->inputSystem->isActionDown(actionId);
}

bool InputAPI::wasActionPressed(int actionId) 
{
    return PulseEngineInstance->inputSystem->wasActionPressed(actionId);
}

bool InputAPI::wasActionReleased(int actionId) 
{
    return PulseEngineInstance->inputSystem->wasActionReleased(actionId);
}

double InputAPI::getMouseX() 
{
    return PulseEngineInstance->inputSystem->getMouseX();
}
double InputAPI::getMouseY() 
{
    return PulseEngineInstance->inputSystem->getMouseY();
}
