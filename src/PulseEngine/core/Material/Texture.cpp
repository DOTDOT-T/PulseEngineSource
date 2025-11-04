#include "Texture.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

Texture::Texture(const std::string &filePath, IGraphicsAPI* graphics)
{
    graphicsAPI = graphics;
    graphicsAPI->GenerateTextureMap(&id, filePath);
    path = filePath;
}


void Texture::Bind(unsigned int slot) const
{
    graphicsAPI->ActivateTexture(slot);
    graphicsAPI->BindTexture(TEXTURE_2D, id);
}

void Texture::Unbind() const
{
    graphicsAPI->BindTexture(TEXTURE_2D, 0);

}
