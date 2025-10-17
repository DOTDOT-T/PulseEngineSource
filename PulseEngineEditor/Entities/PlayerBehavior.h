#pragma once

#include "IScripts.h"
#include "Common/dllExport.h"
#include "Engine.h"

#include "PulseEngine/core/Input/InputSystem.h"
#include "camera.h"

class PULSE_ENGINE_DLL_API PlayerBehavior : public IScript
{
    public:
        PlayerBehavior()
        {
            // if you need to expose variables, do it here with these macro : 
            //AddExposedVariable(EXPOSE_VAR(speed, INT));
            //REGISTER_VAR(speed); //both are needed to make the variable exposed in the editor
        }
        void OnStart() override;
        void OnUpdate() override;
        void OnRender() override;
        const char* GetName() const override;

        InputSystem* inputSystem = nullptr;
        Camera* camera = nullptr;
};

