#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include "Common/common.h"
#include "Common/dllExport.h"

#include  "PulseEngine/ModuleLoader/IModuleInterface/IModuleInterface.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"


class Mesh;
class RenderableMesh;
class SkeletalMesh;
class StaticMesh;
class EntityEditor : public IModuleInterface
{
    public:
        std::string GetName() const override;
        std::string GetVersion() const override;
        void Initialize() override;
        void Shutdown() override;
        void Render() override;

        void SaveEntityToFile();

        void ManageCamera();

        void EntityMeshesManager();

        void AddMeshToEntity();

        void EntityScriptManager();

        void AddScriptToEntity();

        void AddScriptOneNTITY();

        void NewFileClicked(const ClickedFileData& data);
        
        Camera* cam;
        Entity* selectedEntity;
        RenderableMesh* selectedData = nullptr;
        Shader* forRender;
        PulseEngine::Vector2 previewData;
        float distCam = 5.0f;
        bool isAddingToEntity = false;
        bool isSeekingScripts = false;
        bool isAddingMeshes = false;
        std::vector<ContextMenuItem> scriptsContextMenu;
        std::vector<ContextMenuItem> meshesContextMenu;
        IScript* currentScript = nullptr;
};

#endif