/**
 * @file InterfaceEditor.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#ifndef INTERFACE_EDITOR_H
#define INTERFACE_EDITOR_H

#include "pch.h"

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/NodeMenuRegistry.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/Node.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "PulseEngine/core/PulseScript/PulseScript.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include <unordered_map>
#include <filesystem>
#include <thread>
#include <mutex>

#include "zep.h"
#include "zep/imgui/editor_imgui.h"
#include "zep/imgui/display_imgui.h"
#include "zep/imgui/console_imgui.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>


class PulseEngineBackend;
class TopBar;
class Entity;
class IModuleInterface;
class Texture;
class Synapse;
class NewFileManager;
class PulseScriptsManager;
class WidgetEditor;
class Viewport;
class Console;
class EntityEditor;
class MaterialEditor;
class IScript;

struct LoadingPopupData
{
    std::string title;
    std::function<void()> contentFunction;
    float progressPercent = 0.0f;
};


Entity *Instantiate(const std::string &path, PulseEngine::Vector3 position, PulseEngine::Vector3 rotation, PulseEngine::Vector3 scale);

class PULSE_ENGINE_DLL_API InterfaceEditor
{
private:
    friend class TopBar;
    TopBar* topbar;
    Entity* selectedEntity = nullptr;
    std::vector<LoadingPopupData> loadingPopups;
    bool hasProjectSelected = true;
    std::vector<IModuleInterface*> modules;
    PulseScriptsManager* modulesPulseScript = nullptr;
    Texture* folderIcon;
    Texture* fileIcon;
    NewFileManager* newFileManager;

    std::unordered_map<std::string, Texture*> icons;
    std::unordered_map<std::string, unsigned int> texturesLoaded;
    Synapse* synapse = nullptr;

    std::vector<std::pair<Entity*, PulseEngine::Transform*>> reparent;

    WidgetEditor* wdgtEditor = nullptr;
    EntityEditor* entEditor = nullptr;
    MaterialEditor* matEditor = nullptr;

    Viewport* vp = nullptr;
    Console* csl = nullptr;


public:
    std::unordered_map<std::string, bool> windowStates;

    InterfaceEditor();
    ~InterfaceEditor();

    void InitAfterEngine();

#pragma region Main Windows renderer
    void FileExplorerWindow();
    void EngineConfigWindow();
    void EntityAnalyzerWindow();
    void GenerateSceneDataWindow();
    #pragma endregion
    
    void Render();
    void DrawHierarchyNode(HierarchyEntity* node, Entity*& selectedEntity, std::vector<Entity*>& entitiesToDelete);

    void RenderGizmo(PulseEngine::Transform* transform, PulseEngine::Vector2 viewport, ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE);
    PulseEngine::Transform* GetSelectedGizmo();

    // void CleanUpHierarchy(HierarchyEntity* h);
    // void GenerateHierarchy();
    

    void RenderFullscreenWelcomePanel();

    void ShowFileGrid(const fs::path &currentDir, fs::path &selectedFile);
    void NewFileCreation(const std::filesystem::path &currentDir, std::filesystem::path &selectedFile);
    std::vector<std::function<void(const ClickedFileData&)>> fileClickedCallbacks;

    void ShowLoadingPopup(std::function<void()> contentFunction, float progressPercent);
    void AddLoadingPopup(std::function<void()> contentFunction, float progressPercent, const std::string& title = "")
    {
        LoadingPopupData popupData;
        popupData.contentFunction = contentFunction;
        popupData.progressPercent = progressPercent;
        popupData.title = title.empty() ? "Loading..." : title;

        loadingPopups.push_back(popupData);
    }

    void ChangePorgressIn(std::string originText, float progress)
    {
        for (auto& popup : loadingPopups)
        {
            if (popup.title == originText)
            {
                popup.progressPercent = progress;
                return;
            }
        }
    }

    void ChangeProgressContent(std::function<void()> newContentFunction, const std::string& title)
    {
        for (auto& popup : loadingPopups)
        {
            if (popup.title == title)
            {
                popup.contentFunction = newContentFunction;
                return;
            }
        }
    }

    void RemoveLoadingPopup(const std::string& title)
    {
        loadingPopups.erase(std::remove_if(loadingPopups.begin(), loadingPopups.end(),
            [&title](const LoadingPopupData& popup) { return popup.title == title; }), loadingPopups.end());
    }

    Entity* GetSelectedEntity() const
    {
        return selectedEntity;
    }

    std::unique_ptr<Zep::ZepEditor_ImGui> editor;
    fs::path currentDir = "PulseEngineEditor";
    fs::path selected;
    std::mutex m_texturesMutex;
};


#endif