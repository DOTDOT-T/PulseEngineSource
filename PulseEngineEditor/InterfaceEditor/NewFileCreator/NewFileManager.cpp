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
#include "PulseEngine/core/Material/ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"

#include <filesystem>

NewFileManager::NewFileManager()
{
    categories = {
        CategoryFiles("Scripts", {{"New Component", ".cpp/.h"}}),
        CategoryFiles("Assets", {{"Scene (.map)", ".pmap"}, {"Material", ".mat"}, {"Widget", ".widget"}, {"Shader", ".shader"}}),
        CategoryFiles("Entities", {{"Pulse Entity", ".pEntity"}}),
        CategoryFiles("Synapse", {{"Synapse File", ".synapse"}}),
        CategoryFiles("Directory", {{"Simple directory", "DIR"}})};
}

void NewFileManager::OpenPopup()
{

    ImGui::OpenPopup("Add New File");
}

void NewFileManager::RenderPopup(const std::filesystem::path &currentDir, std::filesystem::path &selectedFile)
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImVec2 fullSize = viewport->WorkSize;
    ImVec2 fullPos = viewport->WorkPos;

    ImVec2 winSize = fullSize * 0.5f;
    ImVec2 winPos = ImVec2(
        fullPos.x + (fullSize.x - winSize.x) * 0.5f,
        fullPos.y + (fullSize.y - winSize.y) * 0.5f);

    ImGui::SetNextWindowPos(winPos);
    ImGui::SetNextWindowSize(winSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::BeginPopupModal("Add New File", nullptr, flags))
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
            if (pos != std::string::npos)
            {
                dirStr.erase(pos, toRemove.length());
                sanitizedDir = std::filesystem::path(dirStr);
            }
            toRemove = "PulseEngineEditor\\";
            pos = dirStr.find(toRemove);
            if (pos != std::string::npos)
            {
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
            else if (extension == ".shader")
            {
                fs::path newFilePath = currentDir / (fileNameStr + ".vert");

                if (!fs::exists(newFilePath))
                {
                    std::ofstream ofs(newFilePath.string());
                    if (ofs)
                    {
                        ofs <<
                    R"(#version 330 core
                    
                    layout(location = 0) in vec3 aPos;
                    layout(location = 1) in vec3 aNormal;
                    layout(location = 2) in ivec4 a_BoneIDs;
                    layout(location = 3) in vec3 aTangent;
                    layout(location = 4) in vec2 aTexCoords;
                    layout(location = 5) in vec4 a_BoneWeights;
                    layout(location = 6) in vec3 aBitangent;
                    
                    out vec3 FragPos;
                    out vec3 Normal;
                    out vec2 TexCoords;
                    out vec3 Tangent;
                    out vec3 Bitangent;
                    
                    uniform mat4 model;
                    uniform mat4 view;
                    uniform mat4 projection;
                    
                    uniform bool hasSkeleton;
                    uniform float internalClock;
                    
                    #define MAX_BONES 128
                    uniform mat4 u_BoneMatrices[MAX_BONES];
                    
                    void main()
                    {
                        mat4 skinMatrix = mat4(1.0);
                        FragPos = vec3(model * vec4(aPos, 1.0));
                        Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
                        Tangent = normalize(mat3(model) * aTangent);
                        Bitangent = normalize(mat3(model) * aBitangent);
                        TexCoords = aTexCoords;
                    
                        if(hasSkeleton)
                        {
                            skinMatrix = 
                                a_BoneWeights.x * u_BoneMatrices[int(a_BoneIDs.x)] +
                                a_BoneWeights.y * u_BoneMatrices[int(a_BoneIDs.y)] +
                                a_BoneWeights.z * u_BoneMatrices[int(a_BoneIDs.z)] +
                                a_BoneWeights.w * u_BoneMatrices[int(a_BoneIDs.w)];
                    
                            vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
                            gl_Position = projection * view * model * skinnedPos;
                        }
                        else
                        {
                            gl_Position = projection * view * vec4(FragPos, 1.0);    
                        }
                    }
                    )";
                        ofs.close();
                    }

                }
                newFilePath = currentDir / (fileNameStr + ".frag");

                if (!fs::exists(newFilePath))
                {
                    std::ofstream ofs(newFilePath.string());
                    if (ofs)
                    {
                        ofs <<
                            R"(#version 330 core

                        // Fragment Shader

                        in vec3 FragPos;
                        in vec3 Normal;
                        in vec2 TexCoords;
                        in vec3 Tangent;
                        in vec3 Bitangent;

                        out vec4 FragColor;

                        // === MATERIAL ===
                        uniform vec3 viewPos;
                        uniform vec3 objectColor;

                        uniform sampler2D albedoMap;
                        uniform sampler2D normalMap;
                        uniform sampler2D roughnessMap;

                        uniform float internalClock;

                        // === LIGHT ===
                        struct DirectionalLight
                        {
                            vec3 direction;
                            vec3 target;
                            vec3 color;
                            float intensity;
                            bool castsShadow;
                            float near;
                            float far;
                            sampler2D shadowMap;
                            vec3 position;
                        };

                        uniform DirectionalLight dirLight;

                        void main()
                        {
                            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
                        }
                        )";
                        ofs.close();
                    }
                }

                ShaderManager::GetInstance().RegisterShader(newFilePath.string(), ShaderInfo{fileNameStr, (currentDir / (fileNameStr + ".vert")).string(), (currentDir / (fileNameStr + ".frag")).string(), ""});
                selectedFile = newFilePath;
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
                    DiskArchive *ar = new DiskArchive((sanitizedDir / (fileNameStr + extension)).string(), Archive::Mode::Saving);
                    int entityCount = 0;
                    std::string receivedMapName = fileNameStr;
                    ar->Serialize("sceneName", receivedMapName);
                    uint64_t g = static_cast<uint64_t>(guid);
                    ar->Serialize("guid", g);
                    ar->Serialize("entitiesCount", entityCount);
                    ar->Finalize();
                    delete ar;
                }
                if (extension == ".widget")
                {
                    DiskArchive *ar = new DiskArchive((sanitizedDir / (fileNameStr + extension)).string(), Archive::Mode::Saving);
                    int size = 0;
                    ar->Serialize("size", size);
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

void NewFileManager::RenderFilesTypeColumn(const std::vector<std::pair<std::string, std::string>> &fileTypes)
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

void NewFileManager::GenerateCppAndHeaderFiles(const std::filesystem::path &currentDir, std::string &fileNameStr, std::filesystem::path &selectedFile)
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
            ofs << "void " << fileNameStr << "::OnEditorDisplay()\n";
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
