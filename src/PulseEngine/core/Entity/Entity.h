/**
 * @file Entity.cpp
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @details 
 * It's the start of everything : everything is an Entity.
 * From light, to skeleton, to mesh, to camera, to player, to enemy, to NPC, to anything you want.
 * This is the base class for all entities in the Pulse Engine.
 * This permit an easy init, update, render, and modification by the engine on basic rules.
 * each object that could be rendered, updated, or modified by the engine should inherit from this class.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ENTITY_H
#define ENTITY_H

#include "Common/common.h"
#include "Common/dllExport.h"

#include "PulseEngine/core/Meshes/RenderableMesh.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/SceneManager/HierarchyNode.h"
#include "PulseEngine/core/Math/Transform/Transform.h"

#include <string>
#include <vector>
#include <iostream> // Temporary: consider wrapping with logging macros.

class Mesh;
class Shader;
class Material;
class IScript;
class Collider;
class BoxCollider;
class Archive;

class PulseScriptsManager;

/**
 * @class Entity
 * @brief Represents a scene object with transformation, rendering, and behavior.
 * 
 * Entities are the core unit of composition in the scene and may hold meshes,
 * materials, scripts, and colliders.
 */
class PULSE_ENGINE_DLL_API Entity : public PulseObject
{
    PULSE_GEN_BODY(Entity)
    PULSE_REGISTER_CLASS_HEADER(Entity)
public:

    // ------------------------------------------------------------------------
    // Constructors & Destructor
    // ------------------------------------------------------------------------

    /**
     * @brief Constructs an entity with a mesh and material.
     * @param name The entity's name.
     * @param position The entity's initial world position.
     * @param mesh Pointer to the mesh to render.
     * @param material Pointer to the material for rendering.
     */
    Entity(const std::string &name, const PulseEngine::Vector3 &position, Mesh *mesh, Material *material);

    Entity();

    void BaseConstructor();

    /**
     * @brief Constructs an entity with only a position and name.
     */
    Entity(const std::string& name, const PulseEngine::Vector3& position);

    virtual ~Entity() { }

    // ------------------------------------------------------------------------
    // Transform Setters
    // ------------------------------------------------------------------------

    /// Sets the world position and updates the model matrix.
    void SetPosition(const PulseEngine::Vector3& position) { this->transform.position = position;}

    /// Sets the rotation and updates the model matrix.
    void SetRotation(const PulseEngine::Vector3& rotation) { this->transform.rotation = rotation;}

    /// Sets the scale and updates the model matrix.
    void SetScale(const PulseEngine::Vector3& scale) {this->transform.scale = scale;}

    void SetMaterial(Material* material);

    // ------------------------------------------------------------------------
    // Update & Rendering
    // ------------------------------------------------------------------------

    /**
     * @brief Updates the entity's behavior/scripts.
     * @param deltaTime Time elapsed since the last frame.
     */
    void UpdateEntity(PulseEngine::Mat4 parentMatrix);

    /**
     * @brief Draws the entity using its current mesh/material/shader.
     */
    void DrawEntity() const;

    /**
     * @brief Binds the shader used by the entity (if any).
     */
    void UseShader() const;

    /**
     * @brief Draws the mesh without applying material/shader logic.
     */
    void SimplyDrawMesh() const;

    /**
     * @brief Calculate the matrix of a mesh, based on the entity's transform and the mesh's local transform.
     * @brief this is used in the update, to know the matrix of each mesh of the entity.
     * 
     * @param mesh 
     */
    void CalculateMeshMatrix(RenderableMesh *const &mesh) const;

    /**
     * @brief Automaticaly bind all the textures of the material to the shader that is actual used.
     * 
     */
    void BindTexturesToShader() const;

    /**
     * @brief Specifically draws the mesh using a provided shader program, usefull for shadow or others drawing style
     * @param shaderProgram 
     */
    void DrawMeshWithShader(Shader* shader) const;

    // ------------------------------------------------------------------------
    // Component Addition
    // ------------------------------------------------------------------------

    /// Adds a mesh to the entity.
    HierarchyNode<RenderableMesh>* AddMesh(RenderableMesh* mesh, RenderableMesh* parent = nullptr);
        
    /// Attaches a behavior script to the entity.
    void AddScript(IScript* script);

    /// Removes a previously attached script.
    void RemoveScript(IScript* script);

    // ------------------------------------------------------------------------
    // Getters
    // ------------------------------------------------------------------------

    const PulseEngine::Vector3 &GetPosition() const { return transform.position; }
    const PulseEngine::Vector3& GetRotation() const {return transform.rotation;}
    const PulseEngine::Vector3& GetScale() const {return transform.scale; }
    const PulseEngine::Mat4& GetMatrix() const { return entityMatrix; }
    const std::size_t& GetGuid() const {return guid;}
    /**
     * @brief The Muid is a unique identifier for the entity within the map.
     * 
     * @return const std::size_t& The Muid of the entity.
     */
    const std::size_t& GetMuid() const {return muid;}
    std::string GetName() const {return name;}
    Material* GetMaterial() {return material; }
    std::vector<IScript*>& GetScripts() {return scripts; }
    std::vector<RenderableMesh*>& GetMeshes() {return meshes; }
    std::vector<HierarchyNode<RenderableMesh>*>& GetMeshesHierarchy() {return meshHierarchy; }
    HierarchyNode<RenderableMesh>* AddMeshHierarchy(RenderableMesh* mesh, HierarchyNode<RenderableMesh>* parent);

    // ------------------------------------------------------------------------
    // Setters
    // ------------------------------------------------------------------------

    void SetGuid(std::size_t g) {guid = g;}
    void SetMuid(std::size_t m) {muid = m;}
    void SetName(const std::string& name) {this->name = name;}

    // ------------------------------------------------------------------------
    // Collider (public for now, consider getter/setter)
    // ------------------------------------------------------------------------
    BoxCollider* collider = nullptr;

    void AddTag(const std::string& tag);
    void RemoveTag(const std::string& tag);

    bool HasTag(const std::string& tag) const;

    /**
     * @brief Move the entity -> actualPostion + direction * deltaTime
     * 
     * @param direction the direction of the entity, the movement wanted in 1 second.
     */
    void Move(const PulseEngine::Vector3& direction);
    /**
     * @brief Rotate the entity on the 3 axes from his actual rotation.
     * -> actualRotation + rotation * deltatime
     * 
     * @param rotation the rotation to add to the entity, wanted in 1 second.
     */
    void Rotate(const PulseEngine::Vector3& rotation);

    PulseEngine::Transform transform;

    void AddPulseScript(const char* scriptName);

private:
    // ------------------------------------------------------------------------
    // Internal Data
    // ------------------------------------------------------------------------
    std::string name;
    std::vector<std::string> tags;

    PulseEngine::Mat4 entityMatrix;

    Material* material = nullptr;
    std::size_t guid = 0;
    std::size_t muid = 0;

    std::vector<RenderableMesh*> meshes;
    std::vector<HierarchyNode<RenderableMesh>*> meshHierarchy;
    std::vector<IScript*> scripts;

    float internalClock = 0.0f;

    /// Updates the entity's world transformation matrix.
    void UpdateModelMatrix(PulseEngine::Mat4 parentMatrix);
    PulseScriptsManager* runtimeScripts = nullptr;
};

#endif // ENTITY_H
