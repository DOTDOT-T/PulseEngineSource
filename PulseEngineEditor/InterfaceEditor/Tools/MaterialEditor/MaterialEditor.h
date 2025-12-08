#ifndef MaterialEditor_H
#define MaterialEditor_H

#include "Common/common.h"
#include "Common/dllExport.h"

#include  "PulseEngine/ModuleLoader/IModuleInterface/IModuleInterface.h"

class Mesh;
class Material;

class MaterialEditor : public IModuleInterface
{
    public:
        std::string GetName() const override;
        std::string GetVersion() const override;
        void Initialize() override;
        void Shutdown() override;
        void Render() override;
        void NotSelectedDesign();
        void TextureSelector(const std::string &textureName);
        void ManageCamera();

        void NewFileClicked(const ClickedFileData& data);

        Camera* cam;
        Entity* materialRenderer;
        Material* materialSelected;
        Shader* forRender;
        PulseEngine::Vector2 previewData;
        std::vector<Entity*> entities;
        float distCam = 5.0f;
};

#endif