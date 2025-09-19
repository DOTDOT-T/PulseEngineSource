/**
 * @file DirectionalLight.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Simple directional light class for Pulse Engine.
 * @brief it inherits from LightData and provides functionality for shadow mapping, and can act like a sun.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Common/common.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "Common/dllExport.h"

class PulseEngineBackend;

class PULSE_ENGINE_DLL_API DirectionalLight : public LightData
{
    public:
        PulseEngine::Vector3 direction;

        unsigned int depthMapFBO;
        unsigned int depthMapTex;

        float nearPlane;
        float farPlane;
        PulseEngine::Vector3 target;

        DirectionalLight(float np, float fp, PulseEngine::Vector3 t, PulseEngine::Vector3 p, PulseEngine::Color c, float i, float a) : LightData(p, c, i, a), nearPlane(np), farPlane(fp), target(t)
        {
            InitShadowMap(1024);
            RecalculateLightSpaceMatrix();
        }

        void InitShadowMap(int resolution);
        void RenderShadowMap(Shader &shader,PulseEngineBackend& scene) override;
        void BindToShader(Shader& shader, int index) override;
        void RecalculateLightSpaceMatrix() override;

};

#endif