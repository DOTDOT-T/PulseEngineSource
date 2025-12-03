#include "Material.h"
#include "shader.h"


Shader *Material::GetShader()
{
    if(!shader) shader = new Shader(std::string(ASSET_PATH) + "EngineConfig/shaders/basic.vert", std::string(ASSET_PATH) + "EngineConfig/shaders/basic.frag", PulseEngineGraphicsAPI);
    return shader;
}

void Material::SetShader(Shader * shader)
    {
        this->shader = shader;
    }
