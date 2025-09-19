/**
 * @file LightManager.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Manage all the lights, render their shadow maps, and bind them to shaders, so the update of the backend won't be involved in the light management.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Common/common.h"
#include "Common/dllExport.h"
class LightData;

/**
 * @brief LightManager is responsible for managing all lights in the scene.
 * 
 */
class PULSE_ENGINE_DLL_API LightManager
{
    public:    
        void RenderAllShadowsMap(PulseEngineBackend& scene);
        static void BindLightsToShader(Shader* shader, PulseEngineBackend* scene, Entity* entity);

};


#endif