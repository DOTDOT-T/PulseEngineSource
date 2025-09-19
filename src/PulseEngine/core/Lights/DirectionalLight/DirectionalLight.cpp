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
    shader.SetMat4("shadowMatrix", lightSpaceMatrix);
    for (Entity* obj : scene.entities)
    {
        shader.SetMat4("model", obj->GetMatrix());
        obj->DrawMeshWithShader(shader.getProgramID());
    }

    PulseEngineGraphicsAPI->UnbindShadowFramebuffer();
}

void DirectionalLight::BindToShader(Shader &shader, int index)
{
    
    PulseEngine::Vector3 direction = PulseEngine::Vector3(target.x - position.x, target.y - position.y, target.z - position.z).Normalized();
    
    shader.SetVec3("dirLight.direction", direction);
    shader.SetVec3("dirLight.color", PulseEngine::Vector3(color.r, color.g, color.b));
    shader.SetFloat("dirLight.intensity", intensity);
    shader.SetBool("dirLight.castsShadow", true);
    shader.SetMat4("dirLight.lightSpaceMatrix", lightSpaceMatrix);
}

void DirectionalLight::RecalculateLightSpaceMatrix()
{
    PulseEngine::Mat4 lightProjection = PulseEngine::MathUtils::Matrix::Orthographic(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 300.0f);
    PulseEngine::Vector3 up = fabs(direction.y) > 0.9f 
        ? PulseEngine::Vector3(0.0f, 0.0f, 1.0f)   // switch if too close
        : PulseEngine::Vector3(0.0f, 1.0f, 0.0f);
    
    PulseEngine::Mat4 lightView = PulseEngine::MathUtils::Matrix::LookAt(position, target, up);
    lightSpaceMatrix = lightProjection * lightView;
}
