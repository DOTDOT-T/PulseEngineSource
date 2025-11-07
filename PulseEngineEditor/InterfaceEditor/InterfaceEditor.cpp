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
#include "PulseEngineEditor/InterfaceEditor/Synapse/NodeMenuRegistry.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/Node.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/Synapse.h"
#include "PulseEngineEditor/InterfaceEditor/NewFileCreator/NewFileManager.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/Math/Transform/Transform.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"
#include "PulseEngine/API/GameEntity.h"
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>


#include <filesystem>
using namespace PulseEngine::FileSystem;

namespace ed = ax::NodeEditor;
namespace fs = std::filesystem;

InterfaceEditor::InterfaceEditor()
{    
    topbar = new TopBar();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io; 

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "PulseEditorGUI.ini";
    
    ImFont* dmSansFont = io.Fonts->AddFontFromFileTTF((std::string(ASSET_PATH) + "fonts/EngineFont.ttf").c_str(), 18.0f);
    io.FontDefault = dmSansFont;    
    ImGui::StyleColorsDark(); 

    synapse = new Synapse("");
    newFileManager = new NewFileManager();

    synapse->Init();


    icons["folder"] = new Texture(("InterfaceEditor/icon/folder.png"), PulseEngineGraphicsAPI);
    icons["file"] = new Texture(("InterfaceEditor/icon/file.png"), PulseEngineGraphicsAPI);
    icons["entityFile"] = new Texture(("InterfaceEditor/icon/entityFile.png"), PulseEngineGraphicsAPI);
    icons["modelFile"] = new Texture(("InterfaceEditor/icon/modelFile.png"), PulseEngineGraphicsAPI);
    icons["scene"] = new Texture(("InterfaceEditor/icon/scene.png"), PulseEngineGraphicsAPI);
    icons["cpp"] = new Texture(("InterfaceEditor/icon/cpp.png"), PulseEngineGraphicsAPI);
    icons["h"] = new Texture(("InterfaceEditor/icon/h.png"), PulseEngineGraphicsAPI);
    icons["synapse"] = new Texture(("InterfaceEditor/icon/synapse.png"), PulseEngineGraphicsAPI);

    ImGui_ImplGlfw_InitForOpenGL(PulseEngineInstance->GetWindowContext()->GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    std::vector<std::string> filenames;

     for (const auto& entry : fs::directory_iterator("./Modules/Interface"))
     {
         if (entry.is_regular_file())
         {
             filenames.push_back(entry.path().filename().string());
         }
     }

     for(auto file : filenames)
     {
         IModuleInterface* module = dynamic_cast<IModuleInterface*>(ModuleLoader::GetModuleFromPath(std::string("./Modules/Interface/") + file));
         if(module)
         {
             modules.push_back(module);
             windowStates[module->GetName()] = false;
         }
     }


    fileClickedCallbacks.push_back(
        [](const ClickedFileData& data) 
        { 
            std::string fullPath = data.name.string();
            if (fullPath.size() >= 8 && fullPath.substr(fullPath.size() - 8) == ".pEntity")
            {
                if (ImGui::Selectable("Add to scene"))
                {
                    std::string instantiatePath = fullPath;
                    std::cout << fullPath << std::endl;
                    // Replace backslashes with forward slashes
                    size_t pos = instantiatePath.find("PulseEngineEditor\\");
                    if (pos != std::string::npos) {
                        instantiatePath.erase(pos, std::string("PulseEngineEditor\\").length());
                    }
                    PulseEngine::GameEntity::Instantiate(instantiatePath, PulseEngine::Vector3(0.0f, 0.0f, 0.0f), PulseEngine::Vector3(0.0f, 0.0f, 0.0f), PulseEngine::Vector3(1.0f, 1.0f, 1.0f));
                }
            }

        });
    

    windowStates["SceneData"] = true;
    windowStates["EntityAnalyzer"] = true;
    windowStates["EngineConfig"] = true;
    windowStates["assetManager"] = true;
    windowStates["viewport"] = true;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.ScrollbarRounding = 6.0f;
    style.ChildRounding = 6.0f;
    
    style.FrameBorderSize = 1.0f;
    style.WindowBorderSize = 1.0f;
    style.TabBorderSize = 1.0f;
    
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(8.0f, 6.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    
    
ImVec4 orangeAccent      = ImVec4(1.00f, 0.55f, 0.20f, 1.00f); // Warm orange
ImVec4 orangeAccentHover = ImVec4(1.00f, 0.65f, 0.30f, 1.00f); // Slightly brighter on hover
ImVec4 orangeAccentActive= ImVec4(1.00f, 0.45f, 0.10f, 1.00f); // Slightly darker on press
    // Couleurs
// Textes
style.Colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.95f, 1.00f, 1.00f); // Blanc bleuté, doux
style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.55f, 0.65f, 1.00f);

// Fond général
style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.04f, 0.05f, 0.07f, 1.00f); // Très sombre
style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.05f, 0.06f, 0.07f, 0.98f);

// Bordures
style.Colors[ImGuiCol_Border]                 = ImVec4(0.18f, 0.20f, 0.24f, 0.50f);
style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

// Cadres
style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);

// Titres et barres
style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.06f, 0.07f, 0.08f, 1.00f);
style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.12f, 0.14f, 1.00f);
style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.03f, 0.03f, 0.04f, 1.00f);

style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.07f, 0.08f, 0.09f, 1.00f);

// Scrollbars
style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.50f);
style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.26f, 0.36f, 0.56f, 0.90f);
style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.36f, 0.46f, 0.66f, 0.90f);
style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.46f, 0.56f, 0.76f, 1.00f);

// Coche et sliders
style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.38f, 0.70f, 1.00f, 1.00f);
style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.45f, 0.75f, 1.00f, 1.00f);

// Boutons
style.Colors[ImGuiCol_Button]                 = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.24f, 0.28f, 0.34f, 1.00f);
style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.30f, 0.36f, 0.42f, 1.00f);

// Headers (ex: TreeNode)
style.Colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.22f, 0.26f, 1.00f);
style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.28f, 0.32f, 0.38f, 1.00f);
style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.34f, 0.40f, 0.48f, 1.00f);

// Séparateurs
style.Colors[ImGuiCol_Separator]              = ImVec4(0.22f, 0.24f, 0.28f, 0.60f);
style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.32f, 0.34f, 0.38f, 1.00f);
style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.38f, 0.40f, 0.46f, 1.00f);

// Resize grip
style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.30f, 0.60f, 1.00f, 0.30f);
style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.30f, 0.60f, 1.00f, 0.60f);
style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.30f, 0.60f, 1.00f, 0.90f);

// Tabs
style.Colors[ImGuiCol_Tab]                    = ImVec4(0.14f, 0.16f, 0.20f, 1.00f);
style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.24f, 0.28f, 0.32f, 1.00f);
style.Colors[ImGuiCol_TabActive]              = orangeAccent;
style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);



NodeMenuRegistry::Get().AddCategory("Input/Keyboard", "OnKeyPressed", []()
{
    // Code to spawn node
});

NodeMenuRegistry::Get().AddCategory("Mathematical/Basic", "Add", []()
{
    // Code to spawn add node
});

}

void RenderMainDockSpace()
{
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    ImGui::Begin("MainDockSpace", &dockspaceOpen, window_flags);

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}

void InterfaceEditor::RenderFullscreenWelcomePanel()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("Welcome", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);

    ImGui::Text("Welcome to Pulse Engine Editor!");
    ImGui::Text("Please select a project to get started.");
    
    if (ImGui::Button("Select Project"))
    {
        // Logic to open project selection dialog
        // For now, we just set hasProjectSelected to true
        hasProjectSelected = true;
    }

    ImGui::End();
}



void InterfaceEditor::Render()
{       
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    synapse->Render();

    RenderMainDockSpace();

    if(!hasProjectSelected)
    {
        RenderFullscreenWelcomePanel();
    }
    else
    {
        for(auto mod : modules)
        {
            if(windowStates[mod->GetName()])
                mod->Render();
        }

        if(windowStates["SceneData"]) GenerateSceneDataWindow();
        if(windowStates["EntityAnalyzer"]) EntityAnalyzerWindow();
        if(windowStates["EngineConfig"]) EngineConfigWindow();

        if(windowStates["assetManager"])
        {
            ImGui::Begin("Asset Manager", &windowStates["assetManager"]);
            static fs::path selectedFile;
            FileExplorerWindow();
            ImGui::End();
        }

        topbar->UpdateBar(PulseEngineInstance, this);

        for(auto& popup : loadingPopups)
        {
            ShowLoadingPopup(popup.contentFunction, popup.progressPercent);
        }
    }

    // ✅ Fin de la frame
    ImGui::Render();
    // ImGui::UpdatePlatformWindows();
    // ImGui::RenderPlatformWindowsDefault();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Render additional viewports (if multi-viewport enabled)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();             // Create/Update additional platform windows
        ImGui::RenderPlatformWindowsDefault();      // Render them
        glfwMakeContextCurrent(backup_current_context);  // Restore original context
    }
}

void InterfaceEditor::EngineConfigWindow()
{

    ImGui::Separator();
    ImGui::Begin("Engine config", &windowStates["EngineConfig"]);
    ImGui::BeginChild("EngineConfigBox", ImVec2(0, 120), true, ImGuiWindowFlags_None);
    ImGui::Text("Engine name: %s", PulseEngineInstance->GetEngineName().c_str());
    ImGui::Text("Engine version: %s", PulseEngineInstance->GetEngineVersion().c_str());
    ImGui::Text("Engine dev month: %s", PulseEngineInstance->GetDevMonth().c_str());
    ImGui::Text("Engine company: %s", PulseEngineInstance->GetCompanyName().c_str());
    ImGui::EndChild();

    static int selectedMapIndex = 0;
    static std::vector<std::string> mapFiles = SceneLoader::GetSceneFiles("PulseEngineEditor/Scenes");

    auto saveConfig = FileManager::OpenEngineConfigFile();

    static char engineNameBuffer[128] = {0};
    static bool nameInitialized = false;
    if (!nameInitialized) {
        std::string currentName = saveConfig["GameData"]["Name"];
        strncpy(engineNameBuffer, currentName.c_str(), sizeof(engineNameBuffer) - 1);
        engineNameBuffer[sizeof(engineNameBuffer) - 1] = '\0';
        nameInitialized = true;
    }
    ImGui::Text("Project Name:");
    if (ImGui::InputText("##ProjectName", engineNameBuffer, sizeof(engineNameBuffer))) {
        saveConfig["GameData"]["Name"] = std::string(engineNameBuffer);
        FileManager::SaveEngineConfigFile(saveConfig);
    }

    ImGui::Text("Launch Map:");

    // Set selectedMapIndex to the value from config on first run
    static bool firstTime = true;
    if (firstTime && !mapFiles.empty())
    {
        std::string firstScene = saveConfig["GameData"]["FirstScene"];
        for (int i = 0; i < mapFiles.size(); ++i)
        {
            if (mapFiles[i] == firstScene)
            {
                selectedMapIndex = i;
                break;
            }
        }
        firstTime = false;
    }

    if (ImGui::BeginCombo("##LaunchMapCombo", mapFiles.empty() ? "None" : mapFiles[selectedMapIndex].c_str()))
    {
        for (int i = 0; i < mapFiles.size(); ++i)
        {
            bool isSelected = (selectedMapIndex == i);
            if (ImGui::Selectable(mapFiles[i].c_str(), isSelected))
            {
                selectedMapIndex = i;
                saveConfig["GameData"]["FirstScene"] = mapFiles[i];
                FileManager::SaveEngineConfigFile(saveConfig);
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}

void InterfaceEditor::EntityAnalyzerWindow()
{
    ImGui::Begin("Entity analyzer");

    if (selectedEntity)
    {
        ImGui::Text("Selected Entity: %s", selectedEntity->GetName().c_str());
        ImGui::Text("guid : %s", std::to_string(selectedEntity->GetGuid()).c_str());
        ImGui::Text("Muid : %s", std::to_string(selectedEntity->GetMuid()).c_str());
        ImGui::Separator();

        PulseEngine::Vector3 position = selectedEntity->GetPosition();
        PulseEngine::Vector3 rotation = selectedEntity->GetRotation();
        PulseEngine::Vector3 scale = selectedEntity->GetScale();
        if (ImGui::TreeNode("Transform"))
        {
            // Position
            ImGui::Text("Position:");
            if (ImGui::DragFloat3("##Position", &(position.x), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                selectedEntity->SetPosition(position);
            }

            // Rotation
            if (ImGui::DragFloat3("##Rotation", &(rotation.x), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                selectedEntity->SetRotation(rotation);            
            }
            // Scale
            ImGui::Text("Scale:");
            if (ImGui::DragFloat3("##Scale", &(scale.x), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                selectedEntity->SetScale(scale);
            }

            ImGui::TreePop();
        }

        //support for lights

        if(auto lightParent = dynamic_cast<LightData*>(selectedEntity))
        {

            if (ImGui::TreeNode("Light Properties"))
            {
                if (DirectionalLight* light = dynamic_cast<DirectionalLight*>(lightParent))
                {
                    ImGui::Text("Light Type: Directional");
                    PulseEngine::Color color = light->color;
                    if (ImGui::ColorEdit3("Color", &color.r))
                    {
                        light->color = color;
                    }
                    float intensity = light->intensity;
                    if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, FLT_MAX, "%.2f"))
                    {
                        light->intensity = intensity;
                    }
                    if (ImGui::DragFloat("Distance", &light->farPlane, 0.1f, 0.0f, FLT_MAX, "%.2f"))
                    {
                        light->farPlane = light->farPlane;
                    }
                }
                if(PointLight* light = dynamic_cast<PointLight*>(lightParent))
                {
                    ImGui::Text("Light Type: Point");
                    PulseEngine::Color color = light->color;
                    if (ImGui::ColorEdit3("Color", &color.r))
                    {
                        light->color = color;
                    }
                    float intensity = light->intensity;
                    if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, FLT_MAX, "%.2f"))
                    {
                        light->intensity = intensity;
                    }
                    if (ImGui::DragFloat("Distance", &light->farPlane, 0.1f, 0.0f, FLT_MAX, "%.2f"))
                    {
                        light->farPlane = light->farPlane;
                    }
                }
                ImGui::TreePop();
            }
        }
        int counter = 0;
        EDITOR_LOG("Number of scripts on entity " << selectedEntity->GetScripts().size());

    ImGui::SeparatorText("Attached Scripts");

    int scriptIndex = 0;

    for (auto& script : selectedEntity->GetScripts())
    {
        if (!script) continue;

        // --- Header ---
             std::string headerLabel = script->GetName() + std::string("###") + script->GetName() + "_" + std::to_string(scriptIndex);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
        ImGui::PushStyleColor(ImGuiCol_Header,        ImVec4(0.20f, 0.25f, 0.30f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.30f, 0.35f, 0.40f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  ImVec4(0.35f, 0.40f, 0.45f, 1.0f));

        bool open = ImGui::TreeNodeEx(headerLabel.c_str(),
                                      ImGuiTreeNodeFlags_Framed |
                                      ImGuiTreeNodeFlags_SpanAvailWidth |
                                      ImGuiTreeNodeFlags_DefaultOpen,
                                      "%s", script->GetName());

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        if (open)
        {
            ImGui::Indent(8.0f);
            ImGui::Spacing();

            if(!script->isEntityLinked)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
                ImGui::TextDisabled("Only on this entity");
                ImGui::PopStyleColor();
            }

            script->OnEditorDisplay();

            // --- Variable listing ---
            int varCounter = 0;
            auto exposedVars = script->GetExposedVariables();

            for (auto& var : exposedVars)
            {
             std::string label = var.name + std::string("###") + script->GetName() + "_" + std::to_string(varCounter);
                ImGui::PushID(varCounter);

                // subtle background
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.18f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));

                switch (var.type)
                {
                    case ExposedVariable::Type::INT:
                        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%s", var.name.c_str());
                        ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragInt("##int", reinterpret_cast<int*>(var.ptr), 1.0f);
                        break;

                    case ExposedVariable::Type::FLOAT:
                        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", var.name.c_str());
                        ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragFloat("##float", reinterpret_cast<float*>(var.ptr), 0.1f);
                        break;
                    case ExposedVariable::Type::FLOAT3:
                        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", var.name.c_str());
                        ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragFloat3("##float3", reinterpret_cast<float*>(var.ptr), 0.1f);
                        break;
                    case ExposedVariable::Type::BOOL:
                        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.4f, 1.0f), "%s", var.name.c_str());
                        ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
                        ImGui::Checkbox("##bool", reinterpret_cast<bool*>(var.ptr));
                        break;

                    case ExposedVariable::Type::STRING:
                        ImGui::TextColored(ImVec4(0.8f, 0.6f, 1.0f, 1.0f), "%s", var.name.c_str());
                        ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
                        ImGui::SetNextItemWidth(-1);
                        ImGui::InputText("##str", reinterpret_cast<char*>(var.ptr), 256);
                        break;
                }

                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::Spacing();
                ImGui::PopID();
                varCounter++;
            }

            ImGui::Unindent(8.0f);
            ImGui::TreePop();
        }

        ImGui::Spacing();
        ImGui::Separator();
        scriptIndex++;
    }

        static bool isSelected = false;
        if (ImGui::Button("Add script"))
        {
            isSelected = true;
        }
        if(isSelected)
        {
            if (ImGui::BeginCombo("Choose a script", "Select a script"))
            {
                for (const auto& [name, createFunc] : ScriptsLoader::scriptMap)
                {
                    if (ImGui::Selectable(name.c_str(), isSelected))
                    {
                        IScript* newScript = createFunc();
                        if (newScript)
                        {
                            selectedEntity->AddScript(newScript); 
                        }
                        isSelected = false;
                    }
                }
                ImGui::EndCombo();
            }
        }
        

    }
    else
    {
        ImGui::Text("No entity selected.");
    }

    ImGui::End();
}

void InterfaceEditor::GenerateSceneDataWindow()
{    
    for(auto& rep : reparent)
    {
        SceneManager::GetInstance()->ChangeEntityParent(rep.first, rep.second);
    }

    reparent.clear();
    ImGui::Begin("Scene Manager");
    // if(ImGui::Button("update")) GenerateHierarchy();

    ImGui::TextColored(ImVec4(0.1f, 0.6f, 0.9f, 1.0f), "Scene Hierarchy");
    ImGui::Separator();

    if (ImGui::Button("Clear Scene", ImVec2(-1, 0)))
    {
        selectedEntity = nullptr;
        PulseEngineInstance->ClearScene();
    }

    ImGui::Spacing();
    ImGui::Text("Scene Contents");
    ImGui::Separator();

    ImGui::BeginChild("SceneEntitiesScroll", ImVec2(0, 300), true);

    std::vector<Entity*> entitiesToDelete;

    // --- Affiche la hiérarchie ---
    DrawHierarchyNode(SceneManager::GetInstance()->GetRoot(), selectedEntity, entitiesToDelete);

    // --- Suppression après parcours ---
    for (auto it = entitiesToDelete.rbegin(); it != entitiesToDelete.rend(); ++it)
    {
        if ((*it)->GetTypeName() == std::string("Entity"))
            PulseEngineInstance->DeleteEntity(*it);
        else if ((*it)->GetTypeName() == std::string("LightData"))
        {
            auto lightIt = std::find(PulseEngineInstance->lights.begin(),
                                     PulseEngineInstance->lights.end(),
                                     static_cast<LightData*>(*it));
            if (lightIt != PulseEngineInstance->lights.end())
                PulseEngineInstance->lights.erase(lightIt);
        }
    }

    ImGui::EndChild();
    ImGui::End();



}
void InterfaceEditor::DrawHierarchyNode(HierarchyEntity* node, Entity*& selectedEntity, std::vector<Entity*>& entitiesToDelete)
{
    if (!node || !node->entity)
        return;

    Entity* entity = node->entity;
    bool isSelected = (selectedEntity == entity);
    bool safeEscape = false;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (node->children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (isSelected)
        flags |= ImGuiTreeNodeFlags_Selected;

    std::string label = entity->GetName() + "##" + std::to_string(reinterpret_cast<uintptr_t>(entity));

    bool nodeOpen = ImGui::TreeNodeEx((void*)entity, flags, "%s", label.c_str());

    // --- Sélection ---
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        selectedEntity = entity;

    // --- Drag source ---
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        // payload type can be anything unique
        ImGui::SetDragDropPayload("HIERARCHY_NODE", &entity, sizeof(Entity*));
        ImGui::Text("Dragging %s", label.c_str());
        ImGui::EndDragDropSource();
    }

    // --- Drag target ---
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Entity*));
            Entity* draggedEntity = *(Entity**)payload->Data;

            // --- Handle hierarchy reparenting ---
            if (draggedEntity != entity)
            {
                SceneManager::GetInstance()->ChangeEntityParent(draggedEntity, &entity->transform);  
                std::pair<Entity*, PulseEngine::Transform*> rep;
                rep.first = draggedEntity;
                rep.second = &entity->transform;
                reparent.push_back(rep);
            }
        }
        ImGui::EndDragDropTarget();
        safeEscape = true;
    }

    // --- Context menu ---
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Destroy"))
        {
            entitiesToDelete.push_back(entity);
            selectedEntity = nullptr;
        }
        ImGui::EndPopup();
    }

    // --- Sub-nodes ---
    if (nodeOpen && !node->children.empty())
    {
        if (!safeEscape)
        {
            // Make a shallow copy of the pointers. This avoids iterator invalidation
            // if ChangeEntityParent mutates the original vector during drag/drop.
            std::vector<HierarchyEntity*> childrenCopy = node->children;
            for (HierarchyEntity* child : childrenCopy)
            {
                // Guard: child might have been removed; check non-null and still valid parent-child relation if needed
                if (!child) continue;
                DrawHierarchyNode(child, selectedEntity, entitiesToDelete);
            }
        }
    
        if (!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
            ImGui::TreePop();
    }

}



void InterfaceEditor::ShowLoadingPopup(std::function<void()> contentFunction, float progressPercent)
{
     // Set the size of the popup
    ImVec2 popupSize = ImVec2(300, 100);
    ImVec2 windowSize = ImGui::GetMainViewport()->Size;
    ImVec2 windowPos = ImVec2(
        windowSize.x - popupSize.x - 10.0f, // 10px padding from right
        windowSize.y - popupSize.y - 10.0f  // 10px padding from bottom
    );

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    
    
    ImGui::Begin("LoadingPopup", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);

    ImGui::Text("Loading...");
    ImGui::ProgressBar(progressPercent, ImVec2(-1, 0.0f));

    if (contentFunction)
    {
        ImGui::Separator();
        contentFunction();
    }

    ImGui::End();
}

void InterfaceEditor::InitAfterEngine()
{
    for(auto& module : modules)
    {
        module->Initialize();
    }
}

void InterfaceEditor::RenderGizmo(PulseEngine::Transform* transform, PulseEngine::Vector2 viewport, ImGuizmo::OPERATION operation)
{

    ImGuizmo::BeginFrame();
    // Ensure we have a valid region
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSize.x <= 0.0f) viewportSize.x = 200.0f;
    if (viewportSize.y <= 0.0f) viewportSize.y = 200.0f;

    // Draw invisible button to capture input (represents the rendered viewport)
    // ImGui::InvisibleButton("ViewportGizmo", viewportSize);
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    // --- Absolute screen rect for gizmo ---
    ImVec2 itemMin = ImGui::GetItemRectMin();
    ImVec2 itemMax = ImGui::GetItemRectMax();
    ImVec2 itemSize = ImVec2(viewport.x, viewport.y);

    // Make sure ImGuizmo draws in the same space as ImGui
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    
    ImGuizmo::SetRect(itemMin.x, itemMin.y, itemSize.x, itemSize.y);

    // --- Prepare matrices ---
    PulseEngine::Mat4 translation = PulseEngine::Mat4::CreateTranslation(transform->position);
    PulseEngine::Mat4 modelPE = translation;

    float model[16];
    PulseEngine::MathUtils::Matrix::ToColumnMajor(modelPE, model);

    // --- Camera matrices ---
    Camera* cam = PulseEngineInstance->GetActiveCamera();
    glm::mat4 view = glm::lookAt(
        glm::vec3(cam->Position.x, cam->Position.y, cam->Position.z),
        glm::vec3(cam->Position.x + cam->Front.x, cam->Position.y + cam->Front.y, cam->Position.z + cam->Front.z),
        glm::vec3(cam->Up.x, cam->Up.y, cam->Up.z)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(cam->Zoom),
        viewport.x / viewport.y,
        0.1f,
        1000.0f
    );
    if (ImGui::IsItemActive() && ImGuizmo::IsOver())
        ImGui::ClearActiveID();


    // --- Manipulate only if viewport hovered or already active ---
    ImGuizmo::Manipulate(glm::value_ptr(view),
                         glm::value_ptr(projection),
                         operation,
                         ImGuizmo::LOCAL,
                         model);

                         

    if(ImGuizmo::IsUsing())
    {
        float t[3], r[3], s[3];
        ImGuizmo::DecomposeMatrixToComponents(model, t, r, s);
        if(operation == ImGuizmo::OPERATION::TRANSLATE) transform->position = { t[0], t[1], t[2] };
        if(operation == ImGuizmo::OPERATION::ROTATE) transform->AddWorldRotation(PulseEngine::Vector3(r[0], r[1], r[2]));
        if(operation == ImGuizmo::OPERATION::SCALE) transform->scale        = { s[0], s[1], s[2] };
    }

    
}


PulseEngine::Transform *InterfaceEditor::GetSelectedGizmo()
{
    return selectedEntity ? &selectedEntity->transform : nullptr;
}

// void InterfaceEditor::CleanUpHierarchy(HierarchyEntity* h)
// {

//     for(auto& child : h->children)
//     {
//         CleanUpHierarchy(child);
//         delete child;
//     }

// }

// void InterfaceEditor::GenerateHierarchy()
// {
//     CleanUpHierarchy(hierarchy);

//     std::unordered_map<PulseEngine::Transform*, HierarchyEntity*> allHierarchy;
//     if(!hierarchy.entity)
//     {
//         hierarchy.entity = new Entity();
//         hierarchy.entity->SetName("RootScene");
//     }

//     for(Entity* ent : PulseEngineInstance->entities)
//     {
//         HierarchyEntity* newHierarchy = new HierarchyEntity;
//         allHierarchy[&ent->transform] = newHierarchy;
//         newHierarchy->entity = ent;
//         if(!ent->transform.parent)
//         {
//             hierarchy.children.push_back(newHierarchy);
//         }
//         else
//         {
//             allHierarchy[ent->transform.parent]->children.push_back(newHierarchy);
//         }
//     }
// }
