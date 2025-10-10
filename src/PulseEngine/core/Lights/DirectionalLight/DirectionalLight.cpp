#include "DirectionalLight.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

void DirectionalLight::InitShadowMap(int resolution)
{
    PulseEngineGraphicsAPI->GenerateShadowMap(&depthMapTex, &depthMapFBO, DEFAULT_SHADOW_MAP_RES, DEFAULT_SHADOW_MAP_RES);
}

void DirectionalLight::RenderShadowMap(Shader &shader, PulseEngineBackend &scene)
{
    PulseEngineGraphicsAPI->BindShadowFramebuffer(&depthMapFBO);
    RecalculateLightSpaceMatrix();
    shader.SetVec3("lightPos", position);
    shader.SetVec3("target", target);
    for (Entity* obj : scene.entities)
    {
        obj->DrawMeshWithShader(&shader);
    }

    PulseEngineGraphicsAPI->UnbindShadowFramebuffer();
}

void DirectionalLight::BindToShader(Shader &shader, int index)
{
    
    PulseEngine::Vector3 direction = (target - position).Normalized();
    
    shader.SetVec3("dirLight.direction", direction);
    shader.SetVec3("dirLight.color", PulseEngine::Vector3(color.r, color.g, color.b));
    shader.SetFloat("dirLight.intensity", intensity);
    shader.SetBool("dirLight.castsShadow", true);
    shader.SetVec3("dirLight.target", target);
    shader.SetVec3("dirLight.position", position);
    shader.SetFloat("dirLight.near", nearPlane);
    shader.SetFloat("dirLight.far", farPlane);
}

void DirectionalLight::RecalculateLightSpaceMatrix()
{
    // PulseEngine::Mat4 lightProjection = PulseEngine::MathUtils::Matrix::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 40.0f);
    // PulseEngine::Vector3 up = PulseEngine::Vector3(0.0f, 1.0f, 0.0f);
    
    // PulseEngine::Mat4 lightView = PulseEngine::MathUtils::Matrix::LookAt(position, target, up);
    // lightSpaceMatrix = lightProjection * lightView;
}
