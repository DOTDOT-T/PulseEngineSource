/**
 * @file PointLight.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Common/common.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "Common/dllExport.h"
#include <array>

/**
 * @brief PointLight class, with specification of 6 shadow map (cubemap) one for each direction.
 * 
 */
class PULSE_ENGINE_DLL_API PointLight : public LightData
{
public:
    float farPlane;
    unsigned int depthMapFBO;
    unsigned int depthCubeMap;
    int shadowResolution;

    PointLight(PulseEngine::Vector3 position, PulseEngine::Color color, float intensity, float attenuation, float farPlane, int shadowResolution = DEFAULT_SHADOW_MAP_RES);

    void BindToShader(Shader& shader, int index) override;
    void RenderShadowMap(Shader &shader, PulseEngineBackend& scene) override;
    void RecalculateLightSpaceMatrix() override;

    const std::array<PulseEngine::Mat4, 6>& GetShadowTransforms() const;

private:
    std::array<PulseEngine::Mat4, 6> shadowTransforms;
};

#endif
