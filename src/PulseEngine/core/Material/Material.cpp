#include "Material.h"
#include "shader.h"


Shader *Material::GetShader()
{
    if(!shader) shader = new Shader(std::string(ASSET_PATH) + "shaders/basic.vert", std::string(ASSET_PATH) + "shaders/basic.frag");
    return shader;
}

void Material::SetShader(Shader * shader)
    {
        this->shader = shader;
    }
