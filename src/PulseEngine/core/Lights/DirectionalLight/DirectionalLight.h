/**
 * @file DirectionalLight.h
 * @author Dorian LEXTERIAQUE
 * @brief Defines a directional light type for the Pulse Engine rendering pipeline.
 * 
 * @details
 * The `DirectionalLight` class represents a light source with a fixed direction,
 * typically used to simulate sunlight or moonlight. It inherits from `LightData`
 * and extends it with additional functionality for shadow map generation and
 * light-space transformation calculations.
 * 
 * Features:
 * - Orthographic shadow mapping support.
 * - Configurable near and far plane for shadow frustum.
 * - Automatic computation of the light-space matrix based on direction and target.
 * 
 * The class interacts closely with the rendering backend (`PulseEngineBackend`)
 * to render shadows and update shader uniforms accordingly.
 * 
 * @section Usage Example
 * @code
 *  DirectionalLight sun(
 *      0.1f, 150.0f,                           // Near and far planes
 *      {0.0f, 0.0f, 0.0f},                     // Target position
 *      {10.0f, 30.0f, 10.0f},                  // Light position
 *      PulseEngine::Color(1.0f, 1.0f, 0.95f),  // Light color
 *      1.0f,                                   // Intensity
 *      0.0f                                    // Attenuation (unused for directional lights)
 *  );
 *  
 *  sun.RenderShadowMap(shader, scene);
 *  sun.BindToShader(shader, 0);
 * @endcode
 * 
 * @note Directional lights do not attenuate with distance and are defined
 *       by direction rather than position. The `target` vector defines the
 *       direction (from position to target).
 * 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @see LightData
 * @see PulseEngineBackend
 * @see Shader
 * @see PointLight
 * @see SpotLight
 * 
 * @copyright
 * Copyright (c) 2025
 */

#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Common/common.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "Common/dllExport.h"

class PulseEngineBackend;

/**
 * @class DirectionalLight
 * @brief Represents a sun-like light source that emits light in a constant direction.
 * 
 * @details
 * Directional lights project parallel light rays and typically cover large areas.
 * This implementation supports depth-based shadow mapping using an orthographic
 * projection matrix.
 */
class PULSE_ENGINE_DLL_API DirectionalLight : public LightData
{
    PULSE_GEN_BODY(DirectionalLight)
    PULSE_REGISTER_CLASS_HEADER(DirectionalLight)
public:
    PulseEngine::Vector3 direction; ///< Light direction vector, normalized.

    unsigned int depthMapFBO; ///< Framebuffer object used for shadow map rendering.
    unsigned int depthMapTex; ///< Texture handle for the depth map.

    float nearPlane; ///< Near plane distance for shadow projection.
    float farPlane;  ///< Far plane distance for shadow projection.
    PulseEngine::Vector3 target; ///< Target point the light is oriented toward.

    /**
     * @brief Construct a new Directional Light object.
     * 
     * @param np Near plane distance for shadow rendering.
     * @param fp Far plane distance for shadow rendering.
     * @param t Target position the light looks at.
     * @param p Light position in world space.
     * @param c Light color.
     * @param i Light intensity.
     * @param a Attenuation factor (unused for directional lights).
     */
    DirectionalLight(
        float np,
        float fp,
        PulseEngine::Vector3 t,
        PulseEngine::Vector3 p,
        PulseEngine::Color c,
        float i,
        float a
    ) : LightData(p, c, i, a), nearPlane(np), farPlane(fp), target(t)
    {
        InitShadowMap(2048);
        RecalculateLightSpaceMatrix();
    }

    DirectionalLight() : LightData() 
    {
        InitShadowMap(2048);
        RecalculateLightSpaceMatrix();
    }

    /**
     * @brief Initializes the shadow map resources (FBO and depth texture).
     * 
     * @param resolution Shadow map resolution (e.g., 1024, 2048).
     */
    void InitShadowMap(int resolution);

    /**
     * @brief Renders the depth map from the light's perspective.
     * 
     * @param shader The shader used for shadow pass rendering.
     * @param scene Reference to the scene backend for geometry traversal.
     */
    void RenderShadowMap(Shader& shader, PulseEngineBackend& scene) override;

    /**
     * @brief Binds light properties and shadow map to the provided shader.
     * 
     * @param shader Target shader program.
     * @param index Light index in shader uniform array.
     */
    void BindToShader(Shader& shader, int index) override;

    /**
     * @brief Recalculates the light-space transformation matrix.
     * 
     * @details
     * This matrix transforms world coordinates into the light’s orthographic
     * projection space and is used during shadow map rendering.
     */
    void RecalculateLightSpaceMatrix() override;
};

#endif // DIRECTIONAL_LIGHT_H
