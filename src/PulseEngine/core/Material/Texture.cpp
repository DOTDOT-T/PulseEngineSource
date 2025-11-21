#include "Texture.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "common/EditorDefines.h"

Texture::Texture(const std::string &filePath, IGraphicsAPI* graphics)
{
    graphicsAPI = graphics;
    graphicsAPI->GenerateTextureMap(&id, std::string(ASSET_PATH) + filePath);
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
