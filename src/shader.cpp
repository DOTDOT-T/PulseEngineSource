#include "shader.h"
#include "Common/common.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    // Charger et compiler les shaders
    EDITOR_LOG("Loading shader from: " << vertexPath << " and " << fragmentPath)
    shaderID = PulseEngineBackend::graphicsAPI->CreateShader(vertexPath, fragmentPath);
    EDITOR_LOG("Shader program linked with ID: " << shaderID)
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath)
{
    // Charger et compiler les shaders
    EDITOR_LOG("Loading shader from: " << vertexPath << " and " << fragmentPath)
    shaderID = PulseEngineBackend::graphicsAPI->CreateShader(vertexPath, fragmentPath, geometryPath);
    EDITOR_LOG("Shader program linked with ID: " << shaderID)
}

void Shader::Use() const
{
    PulseEngineGraphicsAPI->UseShader(shaderID);
}

void Shader::SetMat4(const std::string& name, const PulseEngine::Mat4& mat) const
{
    PulseEngineGraphicsAPI->SetShaderMat4(this, name, mat);
}

void Shader::SetMat3(const std::string &name, const PulseEngine::Mat3 &mat) const
{
    PulseEngineGraphicsAPI->SetShaderMat3(this, name, mat);
}

void Shader::SetVec3(const std::string &name, const PulseEngine::Vector3& vec) const
{
    PulseEngineGraphicsAPI->SetShaderVec3(this, name, vec);
}

void Shader::SetFloat(const std::string &name, float fl) const
{
    PulseEngineGraphicsAPI->SetShaderFloat(this, name, fl);
}

void Shader::SetBool(const std::string &name, bool b) const
{
    PulseEngineGraphicsAPI->SetShaderBool(this, name, b);
}

void Shader::SetInt(const std::string& name, int value) const
{
    PulseEngineGraphicsAPI->SetShaderInt(this, name, value);
}

void Shader::SetIntArray(const std::string& name, const int* values, int count) const
{
    PulseEngineGraphicsAPI->SetShaderIntArray(this, name, values, count);
}

void Shader::SetLightingData(const std::vector<DirectionalLight*>& data) const
{
    int numLights = data.size();
    if (numLights == 0) return;

    // Prepare arrays
    std::vector<PulseEngine::Vector3> positions(numLights);
    std::vector<PulseEngine::Vector3> colors(numLights);
    std::vector<float> intensities(numLights);
    std::vector<float> attenuations(numLights);

    //Shadow data (currently commented out)
    std::vector<int> shadowCasterIndices;
    int shadowTextureBase = 5; // TEXTURE5, 6, 7...

    for (int i = 0; i < numLights; ++i)
    {
        positions[i] = data[i]->GetPosition();
        colors[i] = PulseEngine::Vector3(data[i]->color.r, data[i]->color.g, data[i]->color.b);
        intensities[i] = data[i]->intensity;
        attenuations[i] = data[i]->attenuation;
        SetInt("lightTypes[" + std::to_string(i) + "]", static_cast<int>(0));

        // If you have shadow casters, make sure to process them
        if (data[i]->castsShadow)
        {
            int casterIndex = static_cast<int>(shadowCasterIndices.size());
            shadowCasterIndices.push_back(i);
    
            std::string matName = "lightSpaceMatrices[" + std::to_string(casterIndex) + "]";
            SetMat4(matName.c_str(), data[i]->lightSpaceMatrix);

            PulseEngineGraphicsAPI->ActivateTexture(casterIndex);
            PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, data[i]->depthMapTex);
    
            std::string texName = "shadowMaps[" + std::to_string(casterIndex) + "]";
            SetInt(texName.c_str(), casterIndex); // matches TEXTURE_2D + i
        }
    }

    // Upload light arrays

    PulseEngineGraphicsAPI->SetShaderVec3Array(this, "lightPositions", positions);
    PulseEngineGraphicsAPI->SetShaderVec3Array(this, "lightColors", colors);
    PulseEngineGraphicsAPI->SetShaderFloatArray(this, "lightIntensities", intensities);
    PulseEngineGraphicsAPI->SetShaderFloatArray(this, "lightAttenuations", attenuations);
    PulseEngineGraphicsAPI->SetShaderInt(this, "lightCount", numLights);

    //Upload shadow caster indices (commented out, no shadows for now)
    if (!shadowCasterIndices.empty())
    {
        SetInt("shadowCasterCount", static_cast<int>(shadowCasterIndices.size()));
        SetIntArray("shadowCasterIndices", shadowCasterIndices.data(), static_cast<int>(shadowCasterIndices.size()));
    }
}

