#include "shader.h"
#include "Common/common.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, IGraphicsAPI* graphicApi) {
    // Charger et compiler les shaders
    graphics = graphicApi;
    EDITOR_LOG("Loading shader from: " << vertexPath << " and " << fragmentPath)
    shaderID = graphics->CreateShader(vertexPath, fragmentPath);
    EDITOR_LOG("Shader program linked with ID: " << shaderID)
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath, IGraphicsAPI* graphicApi)
{
    graphics = graphicApi;
    // Charger et compiler les shaders
    EDITOR_LOG("Loading shader from: " << vertexPath << " and " << fragmentPath)
    shaderID = graphics->CreateShader(vertexPath, fragmentPath, geometryPath);
    EDITOR_LOG("Shader program linked with ID: " << shaderID)
}

void Shader::Use() const
{
    graphics->UseShader(shaderID);
}

void Shader::SetMat4(const std::string& name, const PulseEngine::Mat4& mat) const
{
    graphics->SetShaderMat4(this, name, mat);
}

void Shader::SetMat3(const std::string &name, const PulseEngine::Mat3 &mat) const
{
    graphics->SetShaderMat3(this, name, mat);
}

void Shader::SetVec3(const std::string &name, const PulseEngine::Vector3& vec) const
{
    graphics->SetShaderVec3(this, name, vec);
}

void Shader::SetFloat(const std::string &name, float fl) const
{
    graphics->SetShaderFloat(this, name, fl);
}

void Shader::SetBool(const std::string &name, bool b) const
{
    graphics->SetShaderBool(this, name, b);
}

void Shader::SetInt(const std::string& name, int value) const
{
    graphics->SetShaderInt(this, name, value);
}

void Shader::SetIntArray(const std::string& name, const int* values, int count) const
{
    graphics->SetShaderIntArray(this, name, values, count);
}