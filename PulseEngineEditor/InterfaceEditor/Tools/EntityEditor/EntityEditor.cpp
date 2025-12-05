#include "EntityEditor.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceEditor.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/Meshes/RenderableMesh.h"
#include "PulseEngine/core/Meshes/StaticMesh.h"
#include "PulseEngine/core/Meshes/SkeletalMesh.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/API/EngineApi.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/API/GameEntity.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/SceneManager/HierarchyNode.h"
#include "PulseEngine/core/Entity/Entity.h"

#include "Shader.h"
#include "camera.h"
#include <windows.h>
#include "json.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui-node/imgui_node_editor.h"
#include "imgui-node/imgui_node_editor_internal.h"


std::string EntityEditor::GetName() const
{
    return std::string("Entity Editor");
}

std::string EntityEditor::GetVersion() const
{
    return std::string("v0.0.1");
}

void EntityEditor::Initialize()
{
    PulseInterfaceAPI::AddFunctionToFileClickedCallbacks(
        [this](const ClickedFileData& data) { this->NewFileClicked(data); }
    );
    forRender = new Shader(std::string(ASSET_PATH) + "shaders/basic.vert", std::string(ASSET_PATH) +  "shaders/basic.frag", PulseEngineInstance->graphicsAPI);

    ImGui::SetCurrentContext(PulseInterfaceAPI::ImGuiContext());
}

void EntityEditor::Shutdown()
{
}

void EntityEditor::Render()
{
    if (!selectedEntity)
    {
        return;
    }

    if(!cam)
    {
        cam = new Camera();
    }
    if(!forRender)
    {
        Initialize();
    }
    std::vector<Entity*> entitiesToRender;

    if (selectedEntity)
    {
        cam->Position = selectedEntity->GetPosition();
        cam->Position -= cam->Front * distCam; // Move the camera back a bit

        selectedEntity->SetRotation(PulseEngine::Vector3(0.0f, 0.0f, 0.0f)); // Reset rotation for better view
        selectedEntity->UpdateEntity(PulseEngine::MathUtils::Matrix::Identity());
        entitiesToRender.push_back(selectedEntity);
    }

    std::string entName = "";

    if(selectedEntity)
    {
        entName = "- ";
        entName += selectedEntity->GetName();
    }



    PulseInterfaceAPI::OpenWindow(("Entity Editor " + entName + "###EntityEditor").c_str());
        PulseEngine::Vector2 windowSize = PulseInterfaceAPI::GetActualWindowSize();
    PulseEngine::Vector2 oneWindow = PulseEngine::Vector2(windowSize.x / 2 - 20, windowSize.y - 65);
    ManageCamera();

     PulseInterfaceAPI::SameLine();
     PulseInterfaceAPI::RenderCameraToInterface(&previewData, cam, "Entity Editor", PulseEngine::Vector2(windowSize.x * 0.5f - 20, windowSize.y - 65), entitiesToRender, nullptr);
    PulseInterfaceAPI::SameLine();
    PulseInterfaceAPI::BeginChild("entity data", PulseEngine::Vector2(-1.0f, -1.0f), true);
    PulseInterfaceAPI::BeginChild("Entity Properties", PulseEngine::Vector2(-1.0f, windowSize.y *0.5f - 65), true);
    PulseInterfaceAPI::WriteText("Entity Properties");

    if (selectedEntity)
    {
        static bool scriptOpen = true;
        static bool meshesOpen = true;
         if(PulseInterfaceAPI::StartTreeNode("Scripts", &scriptOpen))
         {
             EntityScriptManager();
             PulseInterfaceAPI::EndTreeNode();
         }

        if(PulseInterfaceAPI::StartTreeNode("Meshes", &meshesOpen))
        {
            EntityMeshesManager();
            PulseInterfaceAPI::EndTreeNode();
        }

        Material* currentMaterial = selectedEntity->GetMaterial();
        PulseInterfaceAPI::AddMaterialPreview(currentMaterial, PulseEngine::Vector2(50.0f, 50.0f), "Material Preview");
        

        if (currentMaterial != selectedEntity->GetMaterial())
        {
            selectedEntity->SetMaterial(currentMaterial);
        }
    }


    PulseInterfaceAPI::EndChild();

    PulseInterfaceAPI::BeginChild("Entity specific data modifier", PulseEngine::Vector2(-1.0f, windowSize.y *0.5f - 65), true);
    if (selectedData) 
    {
        PulseInterfaceAPI::AddTransformModifierForMesh(selectedData, "Entity Transform Modifier");
    }
    if(currentScript)
    {
        InterfaceEditor::DisplayScriptEditor(currentScript, 0);
    }
    PulseInterfaceAPI::EndChild();
    if(PulseInterfaceAPI::Button("Add to Entity", PulseEngine::Vector2((windowSize.x * 0.3f - 20) * 0.5f, 0.0f)))
    {
        isAddingToEntity = true;
    }
    PulseInterfaceAPI::SameLine();
    if(PulseInterfaceAPI::Button("Save", PulseEngine::Vector2((windowSize.x * 0.3f - 20) * 0.5f, 0.0f)))
    {
        SaveEntityToFile();
    }

    if(isAddingToEntity)
    {
        PulseInterfaceAPI::OpenContextMenu("adding");
    }
PulseInterfaceAPI::ShowContextMenu("adding",
{
    {
        "Add Script...",
        [&]() mutable
        {
            PulseInterfaceAPI::OpenContextMenu("AddingScripts");
            isAddingToEntity = false;
            isSeekingScripts  = true;
            scriptsContextMenu.clear();

            // ─────────────────────────────────────
            //         HEADER (UE-style)
            // ─────────────────────────────────────
            {
                ContextMenuItem header;
                header.label = "SCRIPTS";
                header.type  = EditorWidgetComponent::TEXT;
                header.style["color"] = { {"r",0.55f}, {"g",0.55f}, {"b",0.58f}, {"a",1.0f} };
                header.style["padding_y"] = 4.0f;
                scriptsContextMenu.push_back(header);

                ContextMenuItem sep;
                sep.type = EditorWidgetComponent::SEPARATOR;
                sep.style["color"] = { {"r",0.20f}, {"g",0.20f}, {"b",0.21f}, {"a",1.0f} };
                scriptsContextMenu.push_back(sep);
            }

            // ─────────────────────────────────────
            //             SCRIPT ITEMS
            // ─────────────────────────────────────
            for (const auto& [name, createFunc] : ScriptsLoader::scriptMap)
            {
                ContextMenuItem item;
                item.label = name;
                item.type  = EditorWidgetComponent::SELECTABLE;

                // Style UE – hover doux, police blanche
                item.style["color"]  = { {"r",0.85f}, {"g",0.88f}, {"b",0.92f}, {"a",1.0f} };
                item.style["hover"]  = { {"r",0.18f}, {"g",0.18f}, {"b",0.19f}, {"a",1.0f} };
                item.style["active"] = { {"r",0.24f}, {"g",0.24f}, {"b",0.25f}, {"a",1.0f} };

                item.onClick = [&, name, createFunc]() mutable
                {
                    IScript* newScript = createFunc();
                    if (newScript)
                    {
                        std::string uniqueString = name + std::to_string(std::time(nullptr));
                        newScript->SetGUID(GenerateGUIDFromPath(uniqueString));
                        selectedEntity->AddScript(newScript);
                    }
                    isSeekingScripts = false;
                };

                scriptsContextMenu.push_back(item);
            }
        },
        EditorWidgetComponent::SELECTABLE
    },

    {
        "Add Mesh...",
        [&]() mutable
        {
            PulseInterfaceAPI::OpenContextMenu("AddingMeshes");
            isAddingToEntity = false;
            isAddingMeshes  = true;
            meshesContextMenu.clear();

            // ─────────────────────────────────────
            //         HEADER (UE-style)
            // ─────────────────────────────────────
            {
                ContextMenuItem header;
                header.label = "MESHES";
                header.type  = EditorWidgetComponent::TEXT;
                header.style["color"] = { {"r",0.55f}, {"g",0.55f}, {"b",0.58f}, {"a",1.0f} };
                header.style["padding_y"] = 4.0f;
                meshesContextMenu.push_back(header);

                ContextMenuItem sep;
                sep.type = EditorWidgetComponent::SEPARATOR;
                sep.style["color"] = { {"r",0.20f}, {"g",0.20f}, {"b",0.21f}, {"a",1.0f} };
                meshesContextMenu.push_back(sep);
            }

            // ─────────────────────────────────────
            //             MESH ITEMS
            // ─────────────────────────────────────
            for (const auto& pr : GuidReader::GetAllAvailableFiles("guidCollectionMeshes.puid"))
            {
                ContextMenuItem mesh;
                mesh.label = pr.second;
                mesh.type  = EditorWidgetComponent::SELECTABLE;

                mesh.style["color"]  = { {"r",0.85f}, {"g",0.88f}, {"b",0.92f}, {"a",1.0f} };
                mesh.style["hover"]  = { {"r",0.18f}, {"g",0.18f}, {"b",0.19f}, {"a",1.0f} };
                mesh.style["active"] = { {"r",0.24f}, {"g",0.24f}, {"b",0.25f}, {"a",1.0f} };

                const std::string guidStr  = pr.first;
                const std::string meshName = pr.second;

                mesh.onClick = [&, guidStr, meshName]() mutable
                {
                    unsigned long long guidValue = 0;

                    try { guidValue = std::stoull(guidStr); }
                    catch (const std::exception& e)
                    {
                        EDITOR_ERROR("Invalid GUID string: " << guidStr << " (" << e.what() << ")");
                        return;
                    }

                    RenderableMesh* newMesh = GuidReader::GetMeshFromGuid(guidValue);
                    if (newMesh)
                    {
                        newMesh->SetGuid(guidValue);
                        newMesh->SetName(meshName);
                        selectedEntity->AddMesh(newMesh);
                        EDITOR_SUCCESS("Mesh[" << meshName << "] loaded.");
                    }
                    else EDITOR_ERROR("Unable to load RenderableMesh from GUID[" << guidValue << "]");

                    isAddingMeshes = false;
                };

                meshesContextMenu.push_back(mesh);
            }
        },
        EditorWidgetComponent::SELECTABLE
    }
});

    if(isSeekingScripts)
    {
        PulseInterfaceAPI::OpenContextMenu("AddingScripts");
    }
    if(isAddingMeshes)
    {
        PulseInterfaceAPI::OpenContextMenu("AddingMeshes");
    }

    PulseInterfaceAPI::ShowContextMenu("AddingScripts",scriptsContextMenu);
    PulseInterfaceAPI::ShowContextMenu("AddingMeshes",meshesContextMenu);
    PulseInterfaceAPI::EndChild();
    PulseInterfaceAPI::CloseWindow();
}

void EntityEditor::SaveEntityToFile()
        {
            std::function<nlohmann::json(HierarchyNode<RenderableMesh>*)> GetHierarchyJson;

            GetHierarchyJson = [&](HierarchyNode<RenderableMesh>* thisMeshHierarchy) -> nlohmann::json
            {
                RenderableMesh* mesh = thisMeshHierarchy->item;
                nlohmann::json meshJson;
                meshJson["Guid"] = std::to_string(mesh->GetGuid());
                meshJson["Position"] = {mesh->transform.position.x, mesh->transform.position.y, mesh->transform.position.z};
                meshJson["Rotation"] = {mesh->transform.rotation.x, mesh->transform.rotation.y, mesh->transform.rotation.z};
                meshJson["Scale"] = {mesh->transform.scale.x, mesh->transform.scale.y, mesh->transform.scale.z};
                meshJson["Name"] = mesh->GetName();
                meshJson["MeshPath"] = mesh->GetName();
            
                // Recurse on children
                for (auto* child : thisMeshHierarchy->children)
                {
                    meshJson["Children"].push_back(GetHierarchyJson(child));
                }
            
                return meshJson;
            };


             if (selectedEntity)
            {
                nlohmann::json entityData;
                entityData["Guid"] = std::to_string(selectedEntity->GetGuid());
            
                for (auto* mesh : selectedEntity->GetMeshesHierarchy())
                {
                    entityData["Meshes"].push_back(GetHierarchyJson(mesh));
                }
            
                for (const auto& script : selectedEntity->GetScripts())
                {
                    nlohmann::json scriptJson;
                    scriptJson["Name"] = script->GetName();
                    scriptJson["Guid"] = script->GetGUID();
                    entityData["Scripts"].push_back(scriptJson);
                }
                if(selectedEntity->GetMaterial()) entityData["Material"] = selectedEntity->GetMaterial()->guid;
            
                std::string filePath = selectedEntity->GetName();
                std::ofstream file(filePath);
                if (file.is_open())
                {
                    file << entityData.dump(4);
                    std::cout << "Entity saved to " << filePath << std::endl;
                }
                else
                {
                    std::cerr << "Failed to open file for saving: " << filePath << std::endl;
                }
            }
            else
            {
                std::cerr << "No entity selected to save." << std::endl;
            }
        }

void EntityEditor::ManageCamera()
{
    if(PulseInterfaceAPI::IsCurrentWindowFocused())
    {
        if(GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            distCam += PulseInterfaceAPI::MouseDelta().y * 0.1f;
        }
        if (selectedEntity && (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
        {
            cam->Yaw += PulseInterfaceAPI::MouseDelta().x * 0.1f;
            cam->Pitch -= PulseInterfaceAPI::MouseDelta().y * 0.1f;
            if (cam->Pitch > 89.0f) cam->Pitch = 89.0f;
            if (cam->Pitch < -89.0f) cam->Pitch = -89.0f;

            cam->Position = PulseEngine::MathUtils::RotateAround(selectedEntity->GetPosition(), cam->Yaw, cam->Pitch, distCam);
            cam->Front = (selectedEntity->GetPosition() - cam->Position).Normalized();

        }
    }
}

void EntityEditor::EntityMeshesManager()
{
    auto &meshes = selectedEntity->GetMeshesHierarchy();
    int counter = 0;
    std::vector<std::pair<HierarchyNode<RenderableMesh>*,HierarchyNode<RenderableMesh>*>> removeFromParent;

std::function<void(HierarchyNode<RenderableMesh>*)> CreateTreeNode = [&](HierarchyNode<RenderableMesh>* node)
        {
            RenderableMesh* mesh = node->item;
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (node->children.empty())
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (selectedData == mesh)
                flags |= ImGuiTreeNodeFlags_Selected;
            bool nodeOpen = ImGui::TreeNodeEx(mesh, flags, (mesh->GetName()).c_str());
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                selectedData = mesh;
                // PulseInterfaceAPI::OpenContextMenu(popupId.c_str());
            }


            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                // payload type can be anything unique
                ImGui::SetDragDropPayload("HIERARCHY_MESH_NODE", &node, sizeof(HierarchyNode<RenderableMesh>*));
                ImGui::Text("Dragging %s", mesh->GetName().c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_MESH_NODE"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(HierarchyNode<RenderableMesh>*));
                    HierarchyNode<RenderableMesh>* draggedMesh = *(HierarchyNode<RenderableMesh>**)payload->Data;

                    if (draggedMesh != node)
                    {
                        removeFromParent.emplace_back(std::pair<HierarchyNode<RenderableMesh>*, HierarchyNode<RenderableMesh>*>{draggedMesh->parent, draggedMesh});
                    
                        // Change le parent
                        draggedMesh->parent = node;
                        node->AddChild(draggedMesh);
                    }
                }

                ImGui::EndDragDropTarget();
            }
            
            if(nodeOpen)
            {
                for(HierarchyNode<RenderableMesh>* nd : node->children)
                {
                    CreateTreeNode(nd);
                }
            }

            if (!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen) && nodeOpen)
                ImGui::TreePop();
        };
    for (HierarchyNode<RenderableMesh>* meshNode : meshes)
    {
        if (!meshNode || !meshNode->item) continue;
        CreateTreeNode(meshNode);
    }

    for(auto pr : removeFromParent)
    {
        if(pr.first != nullptr)
        {
            auto& siblings = pr.first->children;
            auto it = std::find(siblings.begin(), siblings.end(), pr.second);
            if(it != siblings.end()) siblings.erase(it);   
        }
        else
        {
            auto it = std::find(meshes.begin(), meshes.end(), pr.second);
            if(it != meshes.end()) meshes.erase(it);  
        }
    }

}

void EntityEditor::AddMeshToEntity()
{
    static bool isSelected = false;

    if (PulseInterfaceAPI::Button("Add Mesh", PulseEngine::Vector2(-1.0f, 0.0f)))
    {
        isSelected = true;
    }
    if (isSelected)
    {
        if (PulseInterfaceAPI::BeginCombo("Chooseeeeeeeeeee a mesh", "mesh"))
        {
            for (const auto pr : GuidReader::GetAllAvailableFiles("guidCollectionMeshes.puid"))
            {
                if (PulseInterfaceAPI::Selectable(pr.second, false))
                {
                    RenderableMesh *newMesh = GuidReader::GetMeshFromGuid(std::stoull(pr.first));

                    if (newMesh)
                    {
                        EDITOR_INFO("mesh[adress[" << newMesh << "]] loaded with success.")
                        newMesh->SetGuid(std::stoull(pr.first));
                        newMesh->SetName(pr.second);
                        selectedEntity->GetMeshes().push_back(newMesh);
                        selectedEntity->AddMeshHierarchy(newMesh, nullptr);
                    }
                    isSelected = false;
                }
            }
            PulseInterfaceAPI::EndCombo();
        }
    }
}

void EntityEditor::EntityScriptManager()
{
    auto& scripts = selectedEntity->GetScripts();

    static bool hasOpenContextual = false;
    static int currentIndex = -1;


    for (size_t i = 0; i < scripts.size(); /* no ++i here */)
    {
        IScript* script = scripts[i];
        if(!script) continue;
        hasOpenContextual = PulseInterfaceAPI::Selectable(std::string(script->GetName()) + "###Script" + std::to_string(i));

        if (hasOpenContextual)
        {
            PulseInterfaceAPI::OpenContextMenu("Entity Script context menu");
            hasOpenContextual = false;
            currentScript = script;
            currentIndex = static_cast<int>(i);
        }
        if(currentScript == script)
        {
            PulseInterfaceAPI::ShowContextMenu("Entity Script context menu",
            {
                {
                    "Delete",
                    [&]
                    {
                        hasOpenContextual = false;
                        scripts.erase(scripts.begin() + currentIndex);
                        currentIndex = -1;
                        currentScript = nullptr;
                    },
                    EditorWidgetComponent::SELECTABLE
                },
                {
                    "Cancel",
                    [&]
                    {
                        hasOpenContextual = false;
                    },
                    EditorWidgetComponent::SELECTABLE
                }
            });

        }

        ++i; // only increment if we didn’t erase
        
    }
}


void EntityEditor::AddScriptToEntity()
{
    static bool isSelected = false;


    if (PulseInterfaceAPI::BeginCombo("Choose a script", "Select a script"))
    {
        for (const auto &[name, createFunc] : ScriptsLoader::scriptMap)
        {
            if (PulseInterfaceAPI::Selectable(name.c_str(), isSelected))
            {
                IScript *newScript = ScriptsLoader::GetScriptFromCallName(name);

                //let's create a guid from script name and the date it was created
                std::string uniqueString = name + std::to_string(std::time(nullptr));
                newScript->SetGUID(GenerateGUIDFromPath(uniqueString));
                if (newScript)
                {
                    selectedEntity->AddScript(newScript);
                }
                isSelected = false;
            }
        }
        PulseInterfaceAPI::EndCombo();
    }
    
}

void EntityEditor::NewFileClicked(const ClickedFileData &data)
{

    std::string fullPath = data.name.string();
    if (fullPath.size() >= 8 && fullPath.substr(fullPath.size() - 8) == ".pEntity")
    {
        
        if(PulseInterfaceAPI::Selectable("Entity Editor"))
        {      
            PulseInterfaceAPI::ChangeWindowState(this, true);
            if(selectedEntity)
            {
                delete selectedEntity;
                selectedEntity = nullptr;
            }  
            std::ifstream file(fullPath);
            nlohmann::json_abi_v3_12_0::json entityData;
            file >> entityData;
            file.close();
            selectedEntity = new Entity(fullPath, PulseEngine::Vector3(0.0f, 0.0f, 0.0f));
            selectedEntity = GuidReader::GetEntityFromJson(entityData, selectedEntity);
            if (selectedEntity)
            {
                selectedEntity->SetName((data.name).string());
                // selectedEntity->SetMaterial(MaterialManager::loadMaterial(std::string(ASSET_PATH) + "Materials/cube.mat"));
                selectedEntity->SetScale(PulseEngine::Vector3(1.0f, 1.0f, 1.0f));
                std::cout << "Selected entity: " << selectedEntity->GetName() << std::endl;
            }
            else
            {
                std::cerr << "Failed to load entity " << std::endl;
            }
        }
    }
}