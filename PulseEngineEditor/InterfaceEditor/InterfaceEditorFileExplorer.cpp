#include "InterfaceEditor.h"
#include "PulseEngine/core/SceneLoader/SceneLoader.h"
#include "PulseEngineEditor/InterfaceEditor/TopBar.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/Lights/PointLight/PointLight.h"
#include "PulseEngine/ModuleLoader/IModule/IModule.h"
#include "PulseEngine/ModuleLoader/IModuleInterface/IModuleInterface.h"
#include "PulseEngine/ModuleLoader/ModuleLoader.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngineEditor/InterfaceEditor/NewFileCreator/NewFileManager.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/SceneLoader/SceneLoader.h"
#include "PulseEngine/core/GUID/GuidCollection.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "PulseEngineEditor/InterfaceEditor/TopBar.h"

#include <filesystem>

using namespace PulseEngine::FileSystem;
namespace fs = std::filesystem;

void InterfaceEditor::ShowFileGrid(const fs::path& currentDir, fs::path& selectedFile)
{
    static float thumbnailSize = 64.0f;
    static float padding = 16.0f;

    float cellSize = thumbnailSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
    {
        columnCount = 1;
    }

    ImGui::SameLine();

    NewFileCreation(currentDir, selectedFile);

    // if (currentDir.has_parent_path())
    // {
    //     if (ImGui::Button("⬅ .."))
    //     {
    //         selectedFile = currentDir.parent_path();
    //     }
    // }

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto& entry : fs::directory_iterator(currentDir))
    {
        const bool isDir = entry.is_directory();
        std::string name = entry.path().filename().string();

        ImGui::PushID(name.c_str());
        ImGui::BeginGroup();

        ImVec2 iconSize = ImVec2(thumbnailSize, thumbnailSize);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        unsigned int image = icons["folder"]->id;
        if(!isDir)
        {
            switch(FileManager::GetFileType(name))
            {
                case FileType::MESH:
                    image = icons["modelFile"]->id;
                    break;
                case FileType::PULSE_ENTITY:
                    image = icons["entityFile"]->id;
                    break;
                case FileType::MAP:
                    image = icons["scene"]->id;
                    break;
                case FileType::SCRIPT_CPP:
                    image = icons["cpp"]->id;
                    break;
                case FileType::SCRIPT_H:
                    image = icons["h"]->id;
                    break;
                case FileType::SYNAPSE:
                    image = icons["synapse"]->id;
                    break;
                case FileType::TEXTURE:
                {
                    std::string fullPath = entry.path().string();
                    const std::string prefix = "PulseEngineEditor\\";
                    if (fullPath.rfind(prefix, 0) == 0)
                    {
                        fullPath = fullPath.substr(prefix.length());
                    }
                
                    if (texturesLoaded.find(fullPath) == texturesLoaded.end())
                    {
                        Texture* texture = new Texture(fullPath, PulseEngineGraphicsAPI);
                        image = texture->id;
                        texturesLoaded[fullPath] = texture->id;
                        delete texture;
                    }
                    else
                    {
                        image = texturesLoaded[fullPath];
                    }
                }
                break;
                default:
                    image = icons["file"]->id;
                    break;
            }
        }

        // ✅ Image cliquable
        if (ImGui::ImageButton(
            name.c_str(), // string ID unique
            (ImTextureID)(intptr_t)(image),
            iconSize
        ))
        {
            if (isDir)
            {
                selectedFile = entry.path(); // navigate
            }
            else
            {
                ImGui::OpenPopup("File Actions");
                selectedFile = entry.path();
            }
        }

        std::string fullPath = entry.path().string();
        const std::string editorPrefix = "PulseEngineEditor\\";
        if (fullPath.rfind(editorPrefix, 0) == 0)
        {
            fullPath = fullPath.substr(editorPrefix.length());
        }
        fullPath = NormalizePath(fullPath);
        if (ImGui::BeginPopup("File Actions"))
        {
            for (auto& callback : fileClickedCallbacks)
            {
                ClickedFileData clickedFileData;
                clickedFileData.name = entry.path();
                clickedFileData.path = currentDir;
                callback(clickedFileData);
            }
            //special function that are already known because its game engine basis
            {
            {
                if(ImGui::Selectable("Load Scene"))
                {
                    SceneLoader::LoadScene(fullPath, PulseEngineInstance);
                    ImGui::CloseCurrentPopup();
                }
            }
            }
            if (ImGui::Selectable("Delete"))
            {
                std::string guid;
                GuidCollection* locatedAt = nullptr;
                EDITOR_INFO("Deleting " << fullPath << " from the engine ressources.")
                std::cout << fullPath << std::endl;


                for(auto& [key, val] :PulseEngineInstance->guidCollections)
                {
                    guid = val->GetGuidFromFilePath(fullPath);
                    if(!guid.empty())
                    {
                        std::cout << "found the guid to delete -> " << guid << std::endl;
                        locatedAt = val;
                        break;
                    }
                }

                if(guid.empty() && !locatedAt)
                {
                    EDITOR_WARN("File " << fullPath << " wasn't in the engine ressources.")
                    EDITOR_INFO(
                        std::endl << "Be aware that if the file was in fact in the engine it can cause issues." 
                        << std::endl << "If so, you have guid leak in the engine ressource.")
                }
                else
                {
                    EDITOR_SUCCESS("File " << fullPath << " have been deleted from engine ressources.")
                    EDITOR_INFO(std::endl << "Be aware that object that refer to the guid[" << guid << "] will now have a nullptr received from engine ressource manager.")
                    locatedAt->RemoveGuidFromCollection(guid);
                }
                fs::remove(entry.path());
            }
            ImGui::EndPopup();
        }

        // ✅ Hover effect visuel
        if (ImGui::IsItemHovered())
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + iconSize.x, cursorPos.y + iconSize.y),
                                    IM_COL32(255, 255, 255, 20), 6.0f);
            
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                std::error_code ec;
                if (fs::exists(entry.path()))
                {
                    if (fs::is_directory(entry.path()))
                    {
                        fs::remove_all(entry.path(), ec); // recursively deletes directory
                    }
                    else
                    {
                        fs::remove(entry.path(), ec); // deletes single file
                    }
                
                    if (ec)
                    {
                        std::cerr << "Failed to delete " << entry.path() << ": " << ec.message() << std::endl;
                    }
                }
            }
        }

        // ✅ Nom du fichier centré et tronqué
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
        ImGui::TextWrapped("%s", name.c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndGroup();
        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
}

void InterfaceEditor::NewFileCreation(const std::filesystem::path &currentDir, std::filesystem::path &selectedFile)
{
    if (ImGui::Button("+"))
    {
        newFileManager->OpenPopup();
    }

    newFileManager->RenderPopup(currentDir, selectedFile);
}

void InterfaceEditor::FileExplorerWindow()
{


    ImGui::Begin("Asset Manager");

    // Show breadcrumb-style navigation

    // Iterate through each part of the path
    fs::path accumulatedPath;
    int index = 0;

    for (const auto& part : currentDir)
    {
        accumulatedPath /= part;

        // Draw the directory part as a button
        ImGui::Text(part.string().c_str());


        // Draw a separator if not the last element
        if (index < std::distance(currentDir.begin(), currentDir.end()) - 1)
        {
            ImGui::SameLine();
            ImGui::Text("/");
            ImGui::SameLine();
        }

        ++index;
    }
    if (ImGui::Button("back"))
    {    
        if (currentDir.has_parent_path())
        {            
            currentDir = currentDir.parent_path();
            selected.clear();
        }
    }

    ShowFileGrid(currentDir, selected);

    if (!selected.empty())
    {
        if (fs::is_directory(selected))
        {
            currentDir = selected;
            selected.clear(); 
        }
    }


    ImGui::End();
}