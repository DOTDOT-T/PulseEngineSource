#include "Texture.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

Texture::Texture(const std::string &filePath)
{
    PulseEngineGraphicsAPI->GenerateTextureMap(&id, filePath);
    path = filePath;
}


void Texture::Bind(unsigned int slot) const
{
    PulseEngineGraphicsAPI->ActivateTexture(slot);
    PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, id);
}

void Texture::Unbind() const
{
    PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, 0);

}
