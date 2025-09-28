
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
#include <glm/gtc/type_ptr.hpp>


#include <filesystem>

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


    icons["folder"] = new Texture(("InterfaceEditor/icon/folder.png"));
    icons["file"] = new Texture(("InterfaceEditor/icon/file.png"));
    icons["entityFile"] = new Texture(("InterfaceEditor/icon/entityFile.png"));
    icons["modelFile"] = new Texture(("InterfaceEditor/icon/modelFile.png"));
    icons["scene"] = new Texture(("InterfaceEditor/icon/scene.png"));
    icons["cpp"] = new Texture(("InterfaceEditor/icon/cpp.png"));
    icons["h"] = new Texture(("InterfaceEditor/icon/h.png"));
    icons["synapse"] = new Texture(("InterfaceEditor/icon/synapse.png"));

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

    windowStates["EntityAnalyzer"] = true;
    windowStates["viewport"] = true;
    windowStates["EngineConfig"] = true;
    windowStates["SceneData"] = true;
    windowStates["assetManager"] = true;

    



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

    auto saveConfig = FileManager::OpenEngineConfigFile(PulseEngineInstance);

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
        FileManager::SaveEngineConfigFile(PulseEngineInstance, saveConfig);
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
                FileManager::SaveEngineConfigFile(PulseEngineInstance, saveConfig);
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
        ImGui::Text("guid : %s", selectedEntity->GetGuid());
        ImGui::Text("Muid : %s", selectedEntity->GetMuid());
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
            ImGui::Text("Rotation:");
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
        std::cout << "Number of scripts on entity " << selectedEntity->GetScripts().size() << std::endl;
         for (auto& s : selectedEntity->GetScripts())
         {
             int varCounter = 0;
             if(!s->isEntityLinked) ImGui::Text("Only on this entity ! ###OnlyThis%s", s->GetName());
             std::string treeNodeLabel = s->GetName() + std::string("###") + s->GetName() + "_" + std::to_string(counter);
             if (ImGui::TreeNode(treeNodeLabel.c_str()))
             {
                 std::cout << "script variable quantity : " << s->GetExposedVariables().size() << std::endl;
                 for (auto& var : s->GetExposedVariables())
                 {
                     std::string label = var.name + "###" + s->GetName() + "_" + std::to_string(counter) + "_" + var.name + "_" + std::to_string(varCounter);
              
                     switch (var.type)
                     {
                         case ExposedVariable::Type::INT:
                             ImGui::InputInt(label.c_str(), reinterpret_cast<int*>(var.ptr));
                             break;
                         case ExposedVariable::Type::FLOAT:
                             ImGui::InputFloat(label.c_str(), reinterpret_cast<float*>(var.ptr));
                             break;
                         case ExposedVariable::Type::BOOL:
                             ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>(var.ptr));
                             break;
                         case ExposedVariable::Type::STRING:
                             break;
                     }
                 }
                 ImGui::TreePop();
             }
             counter++;
             varCounter++;

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
    ImGui::Begin("Scene Manager");

    ImGui::TextColored(ImVec4(0.1f, 0.6f, 0.9f, 1.0f), "Scene Management");
    ImGui::Separator();

    // Clear Scene Button
    if (ImGui::Button("Clear Scene", ImVec2(-1, 0)))
    {
        selectedEntity = nullptr;
        PulseEngineInstance->ClearScene();
    }

    ImGui::Spacing();
    ImGui::Text("Scene Contents");
    ImGui::Separator();

    // Scroll area for Entities + Lights
    ImGui::BeginChild("SceneEntitiesScroll", ImVec2(0, 300), true);

    // --- ENTITIES SECTION ---
    ImGui::Text("Entities");
    ImGui::Separator();

    // Collect entities to safely delete after iteration
    std::vector<size_t> entitiesToDelete;

    for (size_t i = 0; i < PulseEngineInstance->entities.size(); ++i)
    {
        auto entity = PulseEngineInstance->entities[i];
        if (!entity)
            continue;

        // Compose label with persistent ID: use entity GUID or pointer address
        std::string label = entity->GetName() + "##Entity_" + std::to_string(reinterpret_cast<uintptr_t>(entity));

        // Highlight if selected
        if (selectedEntity == entity)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.2f, 0.7f));
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.75f, 0.95f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.45f, 0.7f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        // Full width button but leave room for delete button on right
        float fullWidth = ImGui::GetContentRegionAvail().x;
        float deleteBtnWidth = 24.0f;
        float entityBtnWidth = fullWidth - deleteBtnWidth - 8.0f; // 8 for spacing

        if (ImGui::Button(label.c_str(), ImVec2(entityBtnWidth, 0)))
        {
            selectedEntity = entity;
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::SameLine();

        // Show delete button only on hover of entity button for cleaner UI
        bool isHovered = ImGui::IsItemHovered();
        ImGui::PushStyleColor(ImGuiCol_Button, isHovered ? ImVec4(0.9f, 0.2f, 0.2f, 1.0f) : ImVec4(0.6f, 0.1f, 0.1f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        if (ImGui::Button("X", ImVec2(deleteBtnWidth, 0)))
        {
            entitiesToDelete.push_back(i);
            if (selectedEntity == entity)
                selectedEntity = nullptr;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
    }

    // Delete entities after loop to avoid invalidating vector while iterating
    for (auto it = entitiesToDelete.rbegin(); it != entitiesToDelete.rend(); ++it)
    {
        PulseEngineInstance->DeleteEntity(PulseEngineInstance->entities[*it]);
    }

    ImGui::Separator();
    ImGui::Spacing();

    // --- LIGHTS SECTION ---
    ImGui::Text("Lights");
    ImGui::Separator();

    std::vector<size_t> lightsToDelete;

    for (size_t i = 0; i < PulseEngineInstance->lights.size(); ++i)
    {
        auto light = PulseEngineInstance->lights[i];
        if (!light)
            continue;

        std::string label = "Light #" + std::to_string(i + 1) + "##Light_" + std::to_string(reinterpret_cast<uintptr_t>(light));

        if (selectedEntity == light)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.7f));
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.75f, 0.95f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.45f, 0.7f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        float fullWidth = ImGui::GetContentRegionAvail().x;
        float deleteBtnWidth = 24.0f;
        float lightBtnWidth = fullWidth - deleteBtnWidth - 8.0f;

        if (ImGui::Button(label.c_str(), ImVec2(lightBtnWidth, 0)))
        {
            selectedEntity = light;
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::SameLine();

        bool isHovered = ImGui::IsItemHovered();
        ImGui::PushStyleColor(
            ImGuiCol_Button, 
            isHovered ? ImVec4(1.0f, 0.5f, 0.0f, 1.0f) : ImVec4(0.6f, 0.3f, 0.0f, 0.7f)
        );
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        std::string deleteButtonLabel = "X##delete" + std::to_string(i);
        if (ImGui::Button(deleteButtonLabel.c_str(), ImVec2(deleteBtnWidth, 0)))
        {
            lightsToDelete.push_back(i);
            if (selectedEntity == light)
                selectedEntity = nullptr;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
    }

    for (auto it = lightsToDelete.rbegin(); it != lightsToDelete.rend(); ++it)
    {
        PulseEngineInstance->lights.erase(PulseEngineInstance->lights.begin() + *it);
    }

    ImGui::EndChild();


    ImGui::End();
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
