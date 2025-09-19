#ifndef PULSE_ENGINE_EDITOR_BUILD_GAME_COROUTINE_H
#define PULSE_ENGINE_EDITOR_BUILD_GAME_COROUTINE_H

#include "PulseEngine/core/coroutine/Coroutine.h"
#include "Common/common.h"

class PulseEngineBackend;
class InterfaceEditor;
class TopBar;

class BuildGameCoroutine : public Coroutine
{
private:
    enum Step {
        Init, Step1, Step2, Step3, Step4, Compile, Done
    } currentStep = Init;

    float timer = 0.0f;

public:
    PulseEngineBackend *engine; 
    InterfaceEditor* editor;
    TopBar* topbar;
    bool Update(float deltaTime) override;
};

#endif