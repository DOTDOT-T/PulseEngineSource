/**
 * @file TopBar.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "TopBar.h"

#include <cstdlib>

// #include "json.hpp"

#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/GUID/GuidCollection.h"
#include "PulseEngine/core/SceneLoader/SceneLoader.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceEditor.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PulseEngine/core/Lights/PointLight/PointLight.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/coroutine/CoroutineManager.h"
#include "PulseEngineEditor/InterfaceEditor/BuildGameCoroutine.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include <windows.h>
#include <commdlg.h>
using namespace PulseEngine::FileSystem;

namespace ed = ax::NodeEditor;

void ResetWorkingDirectoryToExe()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
    std::filesystem::current_path(exeDir);
}


/**
 * @brief Update the top bar of the editor interface. It's actually the render of the bar in ImGui
 * 
 * @param[in] engine pointer to the actual backend, needed to perform modification on entities, lighting, etcetc
 * @param[in] editor the interface, because the topbar isn't part of the interface module, but need to rely on it sometimes.
 */
void TopBar::UpdateBar(PulseEngineBackend* engine, InterfaceEditor* editor)
{
    static bool requestOpenNewMapPopup = false;
    static char newMapName[128] = "";

    // === MENU BAR ===
    if (ImGui::BeginMainMenuBar())
    {
        // === FILE MENU ===

        if(ImGui::BeginMenu("Map"))
        {
            if (ImGui::MenuItem("New map"))
            {
                requestOpenNewMapPopup = true;
                strcpy(newMapName, "");
            }
            if (ImGui::MenuItem("Save"))
            {
                SceneLoader::SaveSceneToFile(engine->actualMapName, engine->actualMapPath, engine);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Build"))
        {

            if (ImGui::MenuItem("Build game"))
            {     
                ///
                BuildGameToWindow(engine, editor);
            }
            if(ImGui::MenuItem("Compile user scripts"))
            {
                ScriptsLoader::FreeDll(); 
                ScriptsLoader::scriptMap.clear();

                SceneLoader::SaveSceneToFile(PulseEngineInstance->actualMapName, PulseEngineInstance->actualMapPath, PulseEngineInstance);
                PulseEngineInstance->ClearScene();
                CompileUserScripts(editor, "CustomScripts.dll");
                ScriptsLoader::LoadDLL();
                SceneLoader::LoadScene(PulseEngineInstance->actualMapPath, PulseEngineInstance);
                
            }

            ImGui::EndMenu();
        }

        // === EDIT MENU ===
        if (ImGui::BeginMenu("Add"))
        {
            
            if (false && ImGui::BeginMenu("Primitive"))
            {
                if (ImGui::MenuItem("Cube"))
                {
                    int counter = 0;
                    std::string baseName = "Cube";
                    std::string finalName = baseName;
                
                    // Find a unique name
                    bool nameExists = true;
                    while (nameExists)
                    {
                        nameExists = false;
                        for (const auto& ent : engine->entities)
                        {
                            if (ent->GetName() == finalName)
                            {
                                nameExists = true;
                                counter++;
                                finalName = baseName + " (" + std::to_string(counter) + ")";
                                break;
                            }
                        }
                    }

                    PulseEngineInstance->guidCollections["guidCollectionEntities.puid"]->InsertFile("Entities/primitiveCube.pEntity");
                
                    // Create entity with unique name
                    Entity* cube = new Entity(
                        finalName,
                        PulseEngine::Vector3(0.0f),
                        Primitive::Cube(),
                        MaterialManager::loadMaterial("Materials/cube.mat")
                    );
                    cube->SetGuid(GenerateGUIDFromPath("Entities/primitiveCube.pEntity"));
                    cube->SetMuid(GenerateGUIDFromPathAndMap(finalName, engine->actualMapPath));
                    engine->entities.push_back(cube);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Point light"))
            {
                engine->lights.push_back(new PointLight(
                    PulseEngine::Vector3(0.0f, 5.0f, 0.0f),
                    PulseEngine::Color(1.0f, 1.0f, 1.0f),
                    5.0f,
                    5.0f,
                    50.0f
                ));
                engine->entities.push_back(engine->lights.back());                
                SceneManager::GetInstance()->InsertEntity(engine->lights.back());

            }
            if (ImGui::MenuItem("Directional light"))
            {
                engine->lights.push_back(new DirectionalLight(
                    1.0f,
                    250.0f,
                    PulseEngine::Vector3(0.0f,0.0f,0.0f),
                    PulseEngine::Vector3(0.0f, 0.0f, 0.0f),
                    PulseEngine::Color(1.0f, 1.0f, 1.0f),
                    1.0f,
                    10.0f
                ));
                engine->entities.push_back(engine->lights.back());                
                SceneManager::GetInstance()->InsertEntity(engine->lights.back());

            }            

            if(ImGui::MenuItem("Import"))
            {
                char filePath[MAX_PATH] = {'\0'};

                OPENFILENAME ofn = {};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = nullptr;
                ofn.lpstrFile = filePath;
                ofn.nMaxFile = sizeof(filePath);
                ofn.lpstrFilter = "3D Models\0*.obj;*.fbx;*.glb\0Images\0*.png;*.jpeg\0All\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_EXPLORER  | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn))
                {
                    // ResetWorkingDirectoryToExe();
                    std::string name = std::string(filePath).substr(std::string(filePath).find_last_of("/\\") + 1);
                    std::string assetPath = editor->currentDir.string() + "/" + name;

                    std::string guidPath;
                    switch(FileManager::GetFileType(filePath))
                    {
                        case FileType::MESH:
                            ImportMesh(editor, name, guidPath, filePath, assetPath);
                            break;
                        case FileType::TEXTURE:
                            {
                                namespace fs = std::filesystem; 
                                fs::current_path(FileManager::workingDirectory);

                                try {
                                    // Ensure the destination folder exists
                                    fs::path destDir = assetPath;
                                    if (!fs::exists(destDir.parent_path())) {
                                        fs::create_directories(destDir.parent_path());
                                    }
                                
                                    // Copy file, overwrite if it exists
                                    fs::copy_file(filePath, destDir, fs::copy_options::overwrite_existing);
                                
                                } catch (const fs::filesystem_error& e) {
                                    std::cerr << "Error copying file: " << e.what() << std::endl;
                                }
                                
                                std::string fileStr = editor->currentDir.string() + "/" + name;
                                std::string prefix = "PulseEngineEditor";
                                DeletePrefix(fileStr, prefix, guidPath);
                                PulseEngineInstance->guidCollections["guidCollectionTextures.puid"]->InsertFile(guidPath);
                                
                            }
                            break;
                    }
                }
            }
            ImGui::EndMenu();
        }

        // === VIEW MENU ===
        if (ImGui::BeginMenu("View"))
        {

            for (auto& win : editor->windowStates)
            {
                if (ImGui::MenuItem(win.first.c_str()))
                {
                    win.second = !win.second;
                }
            }

            ImGui::EndMenu();
        }
        // === HELP MENU ===
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) { /* TODO: About popup */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // === NEW MAP POPUP ===
    if (requestOpenNewMapPopup)
    {
        ImGui::OpenPopup("NewMapPopup");
        requestOpenNewMapPopup = false;
    }

    if (ImGui::BeginPopupModal("NewMapPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter new map name:");
        ImGui::InputText("##mapname", newMapName, IM_ARRAYSIZE(newMapName));

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            std::string mapPath = "Scenes/";
            mapPath += newMapName;

            engine->actualMapPath = mapPath;
            engine->actualMapName = newMapName;

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void TopBar::ImportMesh(InterfaceEditor *editor, std::string &name, std::string &guidPath, char filePath[260], std::string &meshPath)
{
    std::string fileStr = editor->currentDir.string() + "/" + name + ".pmesh";
    std::string prefix = "PulseEngineEditor";
    DeletePrefix(fileStr, prefix, guidPath);

    std::filesystem::current_path(FileManager::workingDirectory);
    PulseEngineInstance->guidCollections["guidCollectionMeshes.puid"]->InsertFile(guidPath);
    std::string copyCommand = "xcopy \"" + std::string(filePath) + "\" \"" + meshPath + "\" /Y";
    CopyFileA(std::string(filePath).c_str(), meshPath.c_str(), FALSE); 

    DeletePrefix(meshPath, prefix, meshPath);
    std::ofstream guidFile(fileStr);
    nlohmann::json_abi_v3_12_0::json guidJson;
    guidJson["Guid"] = PulseEngineInstance->guidCollections["guidCollectionMeshes.puid"]->GetGuidFromFilePath(guidPath);
    guidJson["Name"] = name;
    guidJson["MeshPath"] = meshPath;
    guidJson["CreationDate"] = std::time(nullptr);

    guidFile << guidJson.dump(4);
    guidFile.close();
}

void TopBar::DeletePrefix(std::string &fileStr, std::string &prefix, std::string &guidPath)
{
    if (fileStr.find(prefix) == 0)
    {
        guidPath = fileStr.substr(prefix.length());
    }
    else
    {
        guidPath = fileStr;
    }
    while(guidPath.length() > 0 && guidPath[0] == '\\')
    {
        guidPath.erase(0, 1);
    }
    guidPath = NormalizePath(guidPath);

}

std::string NormalizePath(const std::string& input)
{
    std::string out;
    out.reserve(input.size());

    bool lastWasSlash = false;

    for (char c : input)
    {
        char normalized = (c == '\\') ? '/' : c;

        if (normalized == '/')
        {
            if (lastWasSlash)
                continue;           // skip double slash

            lastWasSlash = true;
        }
        else
        {
            lastWasSlash = false;
        }

        out.push_back(normalized);
    }

    return out;
}


void TopBar::BuildGameToWindow(PulseEngineBackend *engine, InterfaceEditor* editor)
{

    // Create the build coroutine
    std::unique_ptr<Coroutine> buildCoroutine = std::make_unique<BuildGameCoroutine>();

    // Safely cast raw pointer from base Coroutine* to derived BuildGameCoroutine*
    BuildGameCoroutine* co = dynamic_cast<BuildGameCoroutine*>(buildCoroutine.get());
    if (co)
    {
        co->engine = engine;
        co->editor = editor;
        co->topbar = this;
    }
    else
    {
        // Handle error: cast failed
        EDITOR_ERROR("Failed to cast Coroutine* to BuildGameCoroutine*\n");
    }


    // Add the coroutine to the editor
    engine->coroutineManager->Add(std::move(buildCoroutine));
}

void TopBar::CompileUserScripts(InterfaceEditor * editor, std::string output )
{
                    //Lets work on the custom files scripts now
                std::string compiler = "g++";
                std::string stdVersion = "-std=c++17";
                std::string defines = "-DBUILDING_DLL -DPULSE_GRAPHIC_OPENGL -DPULSE_WINDOWS";
                std::string flags = "-shared -Wall -g -mconsole " + defines;
                std::string includeDirs = R"(-IUserScripts -Idist\src\PulseEngine\CustomScripts -Idist\include -Idist\src -Idist/src/dllexport -Ldist)";
                std::string libs = "-Ldist -l:libPulseLib.InputSystem.a -l:libPulseEngine.CoreBackend.a -l:libPulseEngine.Registery.a -l:libPulseEngine.FileSystem.a -l:libPulseEngine.Renderer.a -lws2_32 -lwinmm -lmswsock";

                // Gather source files
                std::string sources;
                for (const auto& entry : std::filesystem::directory_iterator("PulseEngineEditor"))
                {
                    AnalyzeEntry(entry, sources);
                }
            
                // Final compilation command
                std::string compileCommand = compiler + " " + stdVersion +
                                             " -o " + output + " " + sources +
                                             includeDirs + " " + libs + " " + flags;
        
            
                int result = system(compileCommand.c_str());

                editor->ChangePorgressIn("Building Game", 1.0f);
                if (result != 0)
                {
                    EDITOR_ERROR("Compilation failed.\n");
                }
                else
                {
                    EDITOR_LOG("DLL generated: " << output << "\n");
                }
}

void TopBar::AnalyzeEntry(const std::filesystem::directory_entry & entry, std::string &sources)
{
    if (entry.is_directory())
    {
        for (const auto& subEntry : std::filesystem::directory_iterator(entry.path()))
        {
            AnalyzeEntry(subEntry, sources);
        }
    }
    else if (entry.path().extension() == ".cpp")
    {
        sources += entry.path().string() + " ";
    }
}



void TopBar::GenerateExecutableForWindow(PulseEngineBackend* engine)
{
    namespace fs = std::filesystem;

    nlohmann::json engineConfig = FileManager::OpenEngineConfigFile();
    std::string gameName = engineConfig["GameData"]["Name"].get<std::string>();
    std::string gameVersion = engineConfig["GameData"]["version"].get<std::string>();

    // === Scan des .a dans dist/libs ===
    std::string libDir = "dist/libs";
    std::string libsToLink = "";

    if (fs::exists(libDir)) {
        for (const auto& entry : fs::directory_iterator(libDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".a") {
                // On doit enlever "lib" + ".a" pour utiliser -lname
                std::string filename = entry.path().filename().string();

                if (filename.rfind("lib", 0) == 0) {
                    std::string libName = filename.substr(3, filename.size() - 5); 
                    libsToLink += "-l" + libName + " ";
                }
            }
        }
    } else {
        std::cerr << "[ERROR] dist/libs not found.\n";
    }

    // === Construction de la ligne de commande ===
    std::string compileCommand =
        "g++ -std=c++20 -Idist/src -Idist/include dist/main.cpp "
        "-Ldist/Build -Ldist/libs "
        + libsToLink +
        "-lglfw3 -lgdi32 -lopengl32 "
        "-DPULSE_GRAPHIC_OPENGL -DPULSE_WINDOWS "
        "-o Build/Game.exe";

    system(compileCommand.c_str());

    // === Rename final ===
    std::string renameCmd = "rename \"Build\\game.exe\" \"" + gameName + ".exe\"";
    system(renameCmd.c_str());
}

#include <filesystem>
#include <iostream>

void TopBar::CopyDllForWindow()
{
    namespace fs = std::filesystem;

    const fs::path distDir = "dist";
    const fs::path buildDir = "Build";

    if (!fs::exists(distDir)) {
        EDITOR_ERROR("[ERROR] dist/ directory not found.\n")
        return;
    }

    if (!fs::exists(buildDir)) {
        fs::create_directories(buildDir);
    }

    for (const auto& entry : fs::directory_iterator(distDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dll") {
            fs::path dst = buildDir / entry.path().filename();

            try {
                fs::copy_file(entry.path(), dst, fs::copy_options::overwrite_existing);
                std::cout << "[COPY] " << entry.path().filename().string() << "\n";
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to copy " 
                          << entry.path().filename().string() 
                          << ": " << e.what() << "\n";
            }
        }
    }
}


void TopBar::CopyAssetForWindow()
{
    system("xcopy PulseEngineEditor \"Build\\PulseEngineEditor\" /E /I /Y");
    system("xcopy \"Modules\" \"Build\\Modules\" /E /I /Y");
}

void TopBar::GenerateWindowsDirectory()
{
    system("echo === Generating folders for Window ===");
    system("if not exist Build mkdir Build");
    system("if not exist \"Build/assets\" mkdir \"Build/assets\"");
    system("if not exist \"Build/Logs\" mkdir \"Build/Logs\"");
}
