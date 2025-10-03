#include "MaterialApi.h"
#include "PulseEngine/core/Material/Material.h"
#include "shader.h"

MaterialApi::MaterialApi(Material *m) : material(m)
{
}

Shader *MaterialApi::GetShader()
{
    return material->GetShader();
}

void MaterialApi::SetShader(Shader *shader)
{
    material->SetShader(shader);
}

void MaterialApi::ChangeColor(float r, float g, float b)
{
    material->color = PulseEngine::Vector3(r, g, b);
}
