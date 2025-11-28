#include "SceneManager.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Math/MathUtils.h"

#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Lights/LightManager.h"
#include "PulseEngine/core/SceneManager/SpatialPartition/SpatialPartition.h"
#include "PulseEngine/core/SceneManager/SpatialPartition/SimpleSpatial/SimpleSpatial.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Physics/CollisionManager.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "PulseEngine/core/PulseScript/PulseScriptsManager.h"
#include "PulseEngine/core/PulseScript/utilities.h"
#include "PulseEngine/core/PulseScript/PulseScript.h"

#include <algorithm>

SceneManager *SceneManager::GetInstance()
{
    static SceneManager* sm;
    if(!sm)
    {
        sm = new SceneManager;
        sm->spatialPartition = new SimpleSpatialPartition;
    } 
    return sm;
}

void SceneManager::InsertEntity(Entity *entity, PulseEngine::Transform *parent)
{
    HierarchyEntity* newHie = new HierarchyEntity(entity);
    allEntities[&entity->transform] = newHie;
    if(parent) {
        allEntities[parent]->children.push_back(newHie);
        entity->transform.parent = parent;
    }
    else root.children.push_back(newHie);

    spatialPartition->Insert(entity);
}

void SceneManager::ChangeEntityParent(Entity *entity, PulseEngine::Transform *newParent)
{
    if (!entity) { EDITOR_ERROR("ChangeEntityParent: null entity"); return; }

    auto itEnt = allEntities.find(&entity->transform);
    if (itEnt == allEntities.end())
    {
        EDITOR_ERROR("ChangeEntityParent: entity not found in allEntities");
        return;
    }
    HierarchyEntity* entHie = itEnt->second;
    if (!entHie) { EDITOR_ERROR("ChangeEntityParent: HierarchyEntity* null"); return; }

    // --- Remove from old parent (if present) ---
    if (entity->transform.parent)
    {
        auto parentIt = allEntities.find(entity->transform.parent);
        if (parentIt != allEntities.end() && parentIt->second)
        {
            auto &parentChildren = parentIt->second->children;
            auto it = FindEntityInNodeChildren(parentChildren, entity);
            if (it != parentChildren.end())
                parentChildren.erase(it);
            else
                EDITOR_WARN("ChangeEntityParent: entity not found among parent's children");
        }
        else
        {
            EDITOR_WARN("ChangeEntityParent: old parent not found in allEntities");
        }
    }
    else
    {
        auto &childRoot = root.children;
        auto it = FindEntityInNodeChildren(childRoot, entity);
        if (it != childRoot.end())
            childRoot.erase(it);
        else
            EDITOR_WARN("ChangeEntityParent: entity not found in root.children");
    }

    // --- Add to new parent or root ---
    if (newParent)
    {
        auto newParentIt = allEntities.find(newParent);
        if (newParentIt != allEntities.end() && newParentIt->second)
        {
            entity->transform.parent = newParent;
            newParentIt->second->children.push_back(entHie);
        }
        else
        {
            EDITOR_WARN("ChangeEntityParent: newParent not registered -> attach to root");
            entity->transform.parent = nullptr;
            root.children.push_back(entHie);
        }
    }
    else
    {
        entity->transform.parent = nullptr;
        root.children.push_back(entHie);
    }
}

std::vector<HierarchyEntity *>::iterator SceneManager::FindEntityInNodeChildren(std::vector<HierarchyEntity *> & childRoot, Entity * entity)
{
    return std::find_if(
        childRoot.begin(),
        childRoot.end(),
        [&](HierarchyEntity *child)
        {
            return child->entity == entity;
        });
}

void SceneManager::UpdateScene()
{
    UpdateEntityHierarchy(&root, PulseEngine::MathUtils::Matrix::Identity());

    // Update spatial data for moved entities
    /**
     * @note we need a vector with all the moved entities (move from O(N) to O(K) with k the moved entities -> less than 5% of N in average)
     */
    for (auto& [transform, node] : allEntities)
        spatialPartition->Update(node->entity);

    //after moving them, we can check for physics collision, and move them back to their original place if they are colliding.
    for(auto trans : allEntities)
    {
        Entity* entA = trans.second->entity;
        for(auto trans : allEntities)
        {
            Entity* entB = trans.second->entity;
            CollisionManager::ManageCollision(dynamic_cast<Collider*>(entA->collider), dynamic_cast<Collider*>(entB->collider));

        }
    }
}

void SceneManager::RenderScene()
{

    std::vector<Entity*> visible;

    GetEntitiesInFrustum(visible);

    std::vector<Variable> args;
    for(Entity* ent : visible)
    {
        if(dynamic_cast<LightData*>(ent)) continue; //a light cant be rendered to scene (for now)
        Entity* drawable = ent;
        Shader* shader = drawable->GetMaterial()->GetShader();

        shader->Use();
        shader->SetMat4("projection", PulseEngineInstance->projection);
        shader->SetMat4("view", PulseEngineInstance->view);
        shader->SetVec3("viewPos", PulseEngineInstance->GetActiveCamera()->Position);

        LightManager::BindLightsToShader(shader, PulseEngineInstance, drawable);

        // drawable->DrawEntity();
        drawable->DrawMeshWithShader(shader);
                drawable->collider->lineTraceShader->Use();
                drawable->collider->lineTraceShader->SetMat4("view", PulseEngineInstance->view);
                drawable->collider->lineTraceShader->SetMat4("projection", PulseEngineInstance->projection);
                drawable->collider->OnRender();

        ent->runtimeScripts->ExecuteMethodOnEachScript("Render", args);
    }

    // RenderEntityHierarchy(&root);
}

void SceneManager::GetEntitiesInFrustum(std::vector<Entity *> &visible)
{
    Frustum frust;
    frust.ExtractFromMatrix(PulseEngineInstance->view * PulseEngineInstance->projection);
    spatialPartition->Query(frust, visible);
}

void SceneManager::RegenerateHierarchy(MapTransforms MapTransforms)
{
    // CleanUpHierarchy(root);


    for(Entity* ent : PulseEngineInstance->entities)
    {
        HierarchyEntity* newHierarchy = new HierarchyEntity;
        MapTransforms[&ent->transform] = newHierarchy;
        newHierarchy->entity = ent;
        if(!ent->transform.parent)
        {
            root.children.push_back(newHierarchy);
        }
        else
        {
            MapTransforms[ent->transform.parent]->children.push_back(newHierarchy);
        }
    }
}

SceneManager::SceneManager()
{
    root.entity = new Entity();
    root.entity->SetName("RootScene");    
}

void SceneManager::CleanHierarchyFrom(HierarchyEntity *top)
{
    for(auto& child : top->children)
    {
        CleanHierarchyFrom(child);

        auto it = allEntities.find(&child->entity->transform);
        if (it != allEntities.end()) {
            allEntities.erase(it);
        }

        delete child;
        child = nullptr;
    }
    top->children.clear();
    if(top != &root) top = nullptr;
}

void SceneManager::UpdateEntityHierarchy(HierarchyEntity *top, PulseEngine::Mat4 parentMatrix)
{
    if(!top) return;
    top->entity->UpdateEntity(parentMatrix);
    
    for(HierarchyEntity* child : top->children)
    {
        UpdateEntityHierarchy(child, top->entity->GetMatrix());
    }
}

void SceneManager::RenderEntityHierarchy(HierarchyEntity *top)
{
    if(!top || !top->entity || !top->entity->GetMaterial() || !top->entity->GetMaterial()->GetShader()) return;

    Entity* drawable = top->entity;
    Shader* shader = drawable->GetMaterial()->GetShader();

    shader->Use();
    shader->SetMat4("projection", PulseEngineInstance->projection);
    shader->SetMat4("view", PulseEngineInstance->view);
    shader->SetVec3("viewPos", PulseEngineInstance->GetActiveCamera()->Position);

    LightManager::BindLightsToShader(shader, PulseEngineInstance, drawable);

    top->entity->DrawEntity();
    for(HierarchyEntity* child : top->children)
    {
        RenderEntityHierarchy(child);
    }
}
