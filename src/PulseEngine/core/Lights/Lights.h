/**
 * @file Lights.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Base class for light in Pulse Engine.
 * @brief This class is the base for all lights in Pulse Engine, it provides a common interface for all light types.
 * @brief It contains properties like position, color, intensity, attenuation, and whether the light casts shadows.
 * @brief It also provides methods to bind the light data to a shader, render the shadow map, and recalculate the light space matrix.
 * @bug The shadow maps aint working, lights are not casting any shadows yet.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LIGHTS_H
#define LIGHTS_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/Entity/Entity.h"


class PulseEngineBackend;

/**
 * @brief LightData is the base class for all light types.
 * @details It contains properties like position, color, intensity, attenuation, and whether the light casts shadows.
 * @details Each light type (e.g., PointLight, DirectionalLight) will inherit from this class and implement its own specific behavior.
 */
class PULSE_ENGINE_DLL_API LightData: public Entity
{
    public:
        PulseEngine::Mat4 lightSpaceMatrix;
        PulseEngine::Color color;
        float intensity;
        float attenuation;
        bool castsShadow;

        /**
         * @brief Bind the light data to a shader.
         * 
         * @param shader usage of the shader class is needed to easily bind the light to fragment and vertex, depending on the graphic api.
         * @param index some light (like pointlight) can have multiple instances, so we need to bind them to the shader with an index (position in the list of accepted lights), the position in the shader will be like "lights[0].position", "lights[1].position", etc.
         */
        virtual void BindToShader(Shader& shader, int index) = 0;
        /**
         * @brief Render the shadow map for the light.
         * @brief This method will render the scene from the light's perspective to create a depth map, which is used for shadow mapping.
         * 
         * @param shader usage of the shader class is needed to easily bind the light to fragment and vertex, depending on the graphic api.
         * @param scene the scene where the light is rendered, used to access entities and render them in the shadow map.
         */
        virtual void RenderShadowMap(Shader &shader, PulseEngineBackend& scene) = 0;
        virtual void RecalculateLightSpaceMatrix() = 0;
    
        LightData(PulseEngine::Vector3 position, PulseEngine::Color color, float intensity, float attenuation) : Entity("Light", position), color(color), intensity(intensity), attenuation(attenuation), castsShadow(true) {}
};

#endif
