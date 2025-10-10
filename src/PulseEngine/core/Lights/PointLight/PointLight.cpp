#include "PointLight.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Math/MathUtils.h"

PointLight::PointLight(PulseEngine::Vector3 position, PulseEngine::Color color, float intensity, float attenuation, float farPlane, int shadowResolution)
    : LightData(position, color, intensity, attenuation),
      farPlane(farPlane),
      shadowResolution(DEFAULT_SHADOW_MAP_RES)
{
    PulseEngineGraphicsAPI->GenerateDepthCubeMap(&depthMapFBO, &depthCubeMap);
    RecalculateLightSpaceMatrix();
}

void PointLight::RecalculateLightSpaceMatrix()
{
    PulseEngine::Mat4 shadowProj = PulseEngine::MathUtils::PerspectiveMat(PulseEngine::MathUtils::ToRadians(90.0f), 1.0f, 0.1f, farPlane);
    PulseEngine::Vector3 pos(position.x, position.y, position.z);

    
    shadowTransforms[0] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3( 1, 0, 0), PulseEngine::Vector3(0, -1, 0));
    shadowTransforms[1] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3(-1, 0, 0), PulseEngine::Vector3(0, -1, 0));
    shadowTransforms[2] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3(0, 1, 0), PulseEngine::Vector3(0, 0, 1));
    shadowTransforms[3] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3(0,-1, 0), PulseEngine::Vector3(0, 0,-1));
    shadowTransforms[4] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3(0, 0, 1), PulseEngine::Vector3(0, -1, 0));
    shadowTransforms[5] = shadowProj * PulseEngine::MathUtils::Matrix::LookAt(pos, pos + PulseEngine::Vector3(0, 0,-1), PulseEngine::Vector3(0, -1, 0));
}

const std::array<PulseEngine::Mat4, 6>& PointLight::GetShadowTransforms() const
{
    return shadowTransforms;
}

void PointLight::BindToShader(Shader& shader, int index)
{
    std::string prefix = "pointLights[" + std::to_string(index) + "].";

    shader.SetVec3(prefix + "position", position);
    shader.SetVec3(prefix + "color", PulseEngine::Vector3(color.r, color.g, color.b));
    shader.SetFloat(prefix + "intensity", intensity);
    shader.SetFloat(prefix + "attenuation", attenuation);
    shader.SetInt(prefix + "castsShadow", 1); // 1 for true, 0 for false
    shader.SetFloat(prefix + "farPlane", farPlane);
}

void PointLight::RenderShadowMap(Shader &shader, PulseEngineBackend& scene)
{
    // if (!castsShadow) return;

    PulseEngineGraphicsAPI->SpecificStartFrame(depthMapFBO, PulseEngine::Vector2(2048, 2048));

    scene.pointLightShadowShader->Use();
    scene.pointLightShadowShader->SetVec3("lightPos", position);
    scene.pointLightShadowShader->SetFloat("farPlane", farPlane);

    // upload all 6 shadow matrices
    for (int i = 0; i < 6; i++)
    {
        std::string name = "shadowMatrices[" + std::to_string(i) + "]";
        scene.pointLightShadowShader->SetMat4(name, shadowTransforms[i]);
    }
    
    // then render all objects once
    for (Entity* obj : scene.entities)
    {
        scene.pointLightShadowShader->SetMat4("model", obj->GetMatrix());
    
        obj->DrawMeshWithShader(scene.pointLightShadowShader);
    }

    PulseEngineGraphicsAPI->ActivateBackCull();
    PulseEngineGraphicsAPI->EndFrame();
}
