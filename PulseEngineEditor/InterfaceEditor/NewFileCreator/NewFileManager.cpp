#include "NewFileManager.h"
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
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/GUID/GuidCollection.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"
#include "PulseEngine/core/FileManager/Archive/DiskArchive.h"
#include <glm/gtc/type_ptr.hpp>


#include "imgui/imgui.h"

#include <filesystem>

NewFileManager::NewFileManager()
{    
    categories = {
        CategoryFiles("Scripts", {{"New Component", ".cpp/.h"}}),        
        CategoryFiles("Assets", {{"Scene (.map)", ".pmap"}, {"Material", ".mat"}}),
        CategoryFiles("Entities", {{"Pulse Entity", ".pEntity"}}),
        CategoryFiles("Synapse", {{"Synapse File", ".synapse"}}),
        CategoryFiles("Directory", {{"Simple directory", "DIR"}})
    };
}

void NewFileManager::OpenPopup()
{
    ImGui::OpenPopup("Add New File");
}

void NewFileManager::RenderPopup(const std::filesystem::path &currentDir, std::filesystem::path &selectedFile)
{
    if (ImGui::BeginPopupModal("Add New File", nullptr))
    {
        ImGui::Text("Add New File");
        ImGui::Separator();

        ImGui::Columns(2, nullptr, false);

        RenderCategories();

        ImGui::NextColumn();

        // Right column: File types for selected category
        const auto &fileTypes = categories[selectedCategory].fileTypes;
        RenderFilesTypeColumn(fileTypes);

        ImGui::Columns(1);

        ImGui::Separator();
        ImGui::InputText("File Name", newFileName, IM_ARRAYSIZE(newFileName));

        ImGui::Separator();
        if (ImGui::Button("Create"))
        {
            std::string fileNameStr = newFileName;
            std::string extension = fileTypes[selectedFileType].second;

            std::filesystem::path sanitizedDir = currentDir;
            std::string dirStr = sanitizedDir.string();
             std::string toRemove = "PulseEngineEditor/";
            size_t pos = dirStr.find(toRemove);
            if (pos != std::string::npos) {
                dirStr.erase(pos, toRemove.length());
                sanitizedDir = std::filesystem::path(dirStr);
            }
            toRemove = "PulseEngineEditor\\";
            pos = dirStr.find(toRemove);
            if (pos != std::string::npos) {
                dirStr.erase(pos, toRemove.length());
                sanitizedDir = std::filesystem::path(dirStr);
            }
            // size_t guid = GuidReader::InsertIntoCollection((sanitizedDir / (fileNameStr + extension)).string());
            size_t guid = 0;

            // Special case: Component (.cpp + .h)
            if (extension == "DIR")
            {
                std::string cmd = "mkdir ";
                cmd += (currentDir / fileNameStr).string();
                system(cmd.c_str());
            }
            else if (extension == ".cpp/.h")
            {
                GenerateCppAndHeaderFiles(currentDir, fileNameStr, selectedFile);
            }
            else
            {
                // Normal single-file case
                std::string fullFileName = fileNameStr + extension;
                fs::path newFilePath = currentDir / fullFileName;

                if (extension == ".pmap")
                {
                    DiskArchive* ar = new DiskArchive((sanitizedDir / (fileNameStr + extension)).string(), Archive::Mode::Saving);
                    int entityCount = 0;
                    std::string receivedMapName = fileNameStr;
                    ar->Serialize("sceneName", receivedMapName);
                    uint64_t g = static_cast<uint64_t>(guid);
                    ar->Serialize("guid", g);
                    ar->Serialize("entitiesCount", entityCount);
                    ar->Finalize();
                    delete ar;
                }
                if (!fs::exists(newFilePath))
                {
                    std::ofstream ofs(newFilePath.string());
                    if (ofs)
                    {
                        if (extension == ".pEntity")
                        {
                            guid = PulseEngineInstance->guidCollections["guidCollectionEntities.puid"]->InsertFile((sanitizedDir / fullFileName).string());
                            ofs << "{\n    \"Guid\": \"" << guid << "\"\n}\n";
                        }
                        else if (extension == ".mat")
                        {
                            guid = PulseEngineInstance->guidCollections["guidCollectionMaterials.puid"]->InsertFile((sanitizedDir / fullFileName).string());
                            ofs << "{\n    \"name\": \"" << fileNameStr << "\",\n\"guid\": \"" << guid << "\"\n}\n";
                        }
                        else if (extension == ".synapse") 
                            ofs << "{\n    \"guid\": \"" << guid << "\"\n}\n";

                        ofs.close();
                    }
                }
                selectedFile = newFilePath;
            }

            newFileName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            newFileName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void NewFileManager::RenderFilesTypeColumn(const std::vector<std::pair<std::string, std::string>> & fileTypes)
{
        for (int j = 0; j < fileTypes.size(); ++j)
        {
            if (ImGui::Selectable(fileTypes[j].first.c_str(), selectedFileType == j, ImGuiSelectableFlags_DontClosePopups))
            {
                selectedFileType = j;
            }
        }
}

void NewFileManager::RenderCategories()
{
        for (int i = 0; i < categories.size(); ++i)
        {
            if (ImGui::Selectable(categories[i].categoryName.c_str(), selectedCategory == i, ImGuiSelectableFlags_DontClosePopups))
            {
                selectedCategory = i;
                selectedFileType = 0;
            }
        }
}

void NewFileManager::GenerateCppAndHeaderFiles(const std::filesystem::path & currentDir, std::string &fileNameStr, std::filesystem::path & selectedFile)
{
                fs::path cppPath = currentDir / (fileNameStr + ".cpp");
                fs::path hPath = currentDir / (fileNameStr + ".h");

                // Create header
                if (!fs::exists(hPath))
                {
                    std::ofstream ofs(hPath.string());
                    if (ofs)
                    {
                        ofs << "#pragma once\n\n";
                        ofs << "#include \"IScripts.h\"\n";
                        ofs << "#include \"Common/dllExport.h\"\n";
                        ofs << "#include \"Engine.h\"\n\n";
                        ofs << "class PULSE_ENGINE_DLL_API " << fileNameStr << " : public IScript\n";
                        ofs << "{\n";
                        ofs << "    public:\n";
                        ofs << "        " << fileNameStr << "()\n";
                        ofs << "        {\n";
                        ofs << "            // if you need to expose variables, do it here with these macro : \n";
                        ofs << "            // AddExposedVariable(EXPOSE_VAR(speed, INT));\n";
                        ofs << "            // REGISTER_VAR(speed); both are needed to make the variable exposed in the editor\n";
                        ofs << "        }\n";
                        ofs << "        void OnStart() override;\n";
                        ofs << "        void OnUpdate() override;\n";
                        ofs << "        void OnRender() override;\n";
                        ofs << "        void OnEditorDisplay() override;\n";
                        ofs << "        const char* GetName() const override;\n";
                        ofs << "};\n\n";

                        ofs.close();
                    }
                }

                // Create cpp
                if (!fs::exists(cppPath))
                {
                    std::ofstream ofs(cppPath.string());
                    if (ofs)
                    {
                        ofs << "#include \"PulseEngine/CustomScripts/IScripts.h\"\n";
                        ofs << "#include \"PulseEngine/API/EngineApi.h\"\n";
                        ofs << "#include \"" << fileNameStr << ".h\"\n\n";
                        ofs << "void " << fileNameStr << "::OnStart()\n";
                        ofs << "{\n";
                        ofs << "    // Your initialization code here\n";
                        ofs << "}\n\n";
                        ofs << "void " << fileNameStr << "::OnUpdate()\n";
                        ofs << "{\n";
                        ofs << "    // Your update code here\n";
                        ofs << "}\n\n";
                        ofs << "void " << fileNameStr << "::OnRender()\n";
                        ofs << "{\n";
                        ofs << "    // Your render code here\n";
                        ofs << "}\n\n";
                        ofs << "void " << fileNameStr << "::OnEditorDisplay()()\n";
                        ofs << "{\n";
                        ofs << "    // Your render code for editor here\n";
                        ofs << "}\n\n";
                        ofs << "const char* " << fileNameStr << "::GetName() const\n";
                        ofs << "{\n";
                        ofs << "    return \"PulseScript" << fileNameStr << "\";\n";
                        ofs << "}\n\n";
                        ofs << "// Register the script\n";
                        ofs << "extern \"C\" __declspec(dllexport) IScript* PulseScript" << fileNameStr << "()\n";
                        ofs << "{\n";
                        ofs << "    return new " << fileNameStr << "();\n";
                        ofs << "}\n\n";

                        ofs.close();
                    }
                }

                // Optionally set selected file to header
                selectedFile = hPath;
}
