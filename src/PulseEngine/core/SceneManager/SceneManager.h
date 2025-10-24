/**
 * @file SceneManager.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief The class that manage all the scene rendering and update
 * @note will include BSP in the futur
 * @version 0.1
 * @date 2025-10-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "common/common.h"
#include "common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"

class Entity;

struct HierarchyEntity
{
    Entity* entity = nullptr;
    std::vector<HierarchyEntity*> children;

    HierarchyEntity()
    {
        entity = nullptr;
    }

    HierarchyEntity(Entity* ent) 
    {
        entity = ent;
    }
};

typedef std::unordered_map<PulseEngine::Transform*, HierarchyEntity*> MapTransforms;


class PULSE_ENGINE_DLL_API SceneManager : public PulseObject
{
public:
    static SceneManager* GetInstance();

    void InsertEntity(Entity* entity, PulseEngine::Transform* parent = nullptr);
    void ChangeEntityParent(Entity *entity, PulseEngine::Transform *newParent);
    std::vector<HierarchyEntity *>::iterator FindEntityInNodeChildren(std::vector<HierarchyEntity *> &childRoot, Entity *entity);
    HierarchyEntity* GetRoot() {return &root;}

    void UpdateScene();
    void RenderScene();

    
    void RegenerateHierarchy(MapTransforms MapTransforms);

private:
    SceneManager();
    SceneManager(const SceneManager& sm) = delete;
    ~SceneManager() = delete;

    void CleanHierarchyFrom(HierarchyEntity* top);

    void UpdateEntityHierarchy(HierarchyEntity *top, PulseEngine::Mat4 parentMatrix);
    void RenderEntityHierarchy(HierarchyEntity *top);

    MapTransforms allEntities;
    HierarchyEntity root;

};

#endif