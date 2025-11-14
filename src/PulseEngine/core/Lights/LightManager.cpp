#include "LightManager.h"
#include "common/common.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/core/Lights/PointLight/PointLight.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/Material.h"

void LightManager::BindLightsToShader(Shader *shader, PulseEngineBackend* scene, Entity* entity)
{

    // Bind entity-specific uniforms
    shader->SetVec3("objectColor", entity->GetMaterial()->color);

    int maxPointLights = 4; // must match shader array size
    int boundPointLights = 0;

    for (size_t i = 0; i < scene->lights.size() && boundPointLights < maxPointLights; ++i)
    {
        PointLight* pLight = dynamic_cast<PointLight*>(scene->lights[i]);
        if (!pLight)
            continue;

        // Bind PointLight uniforms
        pLight->BindToShader(*shader, boundPointLights);

        PulseEngineGraphicsAPI->ActivateTexture(boundPointLights);
        PulseEngineGraphicsAPI->BindTexture(TEXTURE_CUBE_MAP, pLight->depthCubeMap);

        std::string baseName = "pointLights[" + std::to_string(boundPointLights) + "]";
        shader->SetInt((baseName + ".depthMap"), boundPointLights);

        boundPointLights++;
    }
    // Bind directional light if found
    for (size_t i = 0; i < scene->lights.size(); ++i)
    {
        DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(scene->lights[i]);
        if (!dLight)
            continue;
    
        dLight->BindToShader(*shader, -1);
    
        if (dLight->castsShadow)
        {            PulseEngineGraphicsAPI->ActivateTexture(maxPointLights);
            PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, dLight->depthMapTex);
        
            shader->SetInt("dirLight.shadowMap", maxPointLights);
        }
    
        break; // Only one directional light supported
    }


    shader->SetInt("numPointLights", boundPointLights);
}