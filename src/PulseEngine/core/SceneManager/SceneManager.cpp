#include "SceneManager.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include <algorithm>

SceneManager *SceneManager::GetInstance()
{
    static SceneManager* sm;
    if(!sm)
    {
        sm = new SceneManager;
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
    for(auto obj : root.children)
    {
        obj->entity->UpdateEntity(0.0f);
        UpdateEntityHierarchy(obj, PulseEngine::MathUtils::Matrix::Identity());
    }
}

void SceneManager::RenderScene()
{
    RenderEntityHierarchy(&root);
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
    // for(auto& child : top->children)
    // {
    //     CleanUpHierarchy(child);
    //     delete child;
    // }
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
    if(!top) return;
    top->entity->DrawEntity();
    for(HierarchyEntity* child : top->children)
    {
        RenderEntityHierarchy(child);
    }
}
