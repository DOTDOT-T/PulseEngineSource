#ifndef ENTITY_API_H
#define ENTITY_API_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/CustomScripts/IScripts.h"

class MaterialApi;

namespace PulseEngine
{
    /**
     * @class EntityApi
     * @brief High-level interface used to manipulate a runtime Entity instance.
     *
     * Provides direct access to spatial transforms, component management,
     * physics collider parameters, script manipulation, and mesh-level lookups.
     *
     * This API is designed as the scripting entry point of PulseEngine.
     * It abstracts engine internals while keeping low-level control accessible
     * to custom scripts and gameplay systems.
     */
    class PULSE_ENGINE_DLL_API EntityApi
    {
    private:
        /// Pointer to the underlying engine-side entity.
        Entity* entity;

        /// Lazily created material API interface for this entity.
        MaterialApi* materialApi;

    public:

        /**
         * @brief Constructs an EntityApi wrapper for a given engine entity.
         * @param e Pointer to the engine-side entity instance.
         */
        EntityApi(Entity* e);

        /**
         * @brief Returns the world-space position of the entity.
         * @return A Vector3 representing the entity position.
         */
        PulseEngine::Vector3 GetPosition() { return entity->GetPosition(); }

        /**
         * @brief Returns the world-space rotation of the entity.
         * @return A Vector3 representing rotation (Euler angles).
         */
        PulseEngine::Vector3 GetRotation() { return entity->GetRotation(); }

        /**
         * @brief Sets the world-space position of the entity.
         * @param pos New position to apply.
         */
        void SetPosition(const PulseEngine::Vector3& pos) { entity->SetPosition(pos); }

        /**
         * @brief Sets the world-space rotation of the entity.
         * @param rot Euler angles in degrees.
         */
        void SetRotation(const PulseEngine::Vector3& rot) { entity->SetRotation(rot); }

        /**
         * @brief Applies an incremental rotation to the entity.
         * @param rot Rotation delta to apply (Euler angles).
         */
        void Rotate(const PulseEngine::Vector3& rot)
        {
            entity->SetRotation(entity->GetRotation() + rot);
        }

        /**
         * @brief Translates the entity by the given offset.
         * @param move Translation vector to apply.
         */
        void Move(const PulseEngine::Vector3& move)
        {
            entity->transform.position += move;
        }

        /**
         * @brief Gives access to the underlying Transform struct.
         * @return Pointer to the entity Transform.
         */
        Transform* GetTransform() { return &entity->transform; }

        /**
         * @brief Gives direct access to the raw engine Entity object.
         * @warning Use with care. Prefer API-level functions when possible.
         * @return Pointer to the underlying Entity.
         */
        Entity* GetEntity() { return entity; }

        /**
         * @brief Retrieves the first component of the requested script type.
         *
         * Iterates through all script components attached to the entity and
         * returns the first one that matches the requested class through RTTI.
         *
         * @tparam ClassScript The script class to search for.
         * @return Pointer to the component if found, nullptr otherwise.
         */
        template <typename ClassScript>
        ClassScript* GetComponent()
        {
            for (auto* script : entity->GetScripts())
            {
                if (auto* ptr = dynamic_cast<ClassScript*>(script))
                    return ptr;
            }
            return nullptr;
        }

        /**
         * @brief Retrieves a mesh whose name exactly matches the provided string.
         * @param name The mesh name to search for.
         * @return Pointer to the mesh, or nullptr if not found.
         */
        RenderableMesh* GetMeshByName(const std::string& name)
        {
            for (auto& mesh : entity->GetMeshes())
            {
                if (mesh->GetName() == name)
                    return mesh;
            }
            return nullptr;
        }

        /**
         * @brief Retrieves all meshes whose name contains the given substring.
         * @param namePart Substring to match.
         * @return List of matching meshes (possibly empty).
         */
        std::vector<RenderableMesh*> GetMeshesContainingName(const std::string& namePart)
        {
            std::vector<RenderableMesh*> matchingMeshes;
            for (auto& mesh : entity->GetMeshes())
            {
                if (mesh->GetName().find(namePart) != std::string::npos)
                    matchingMeshes.push_back(mesh);
            }
            return matchingMeshes;
        }

        /**
         * @brief Retrieves the first mesh whose name contains the specified substring.
         * @param namePart Substring to look for.
         * @return Pointer to the first matching mesh, or nullptr if none.
         */
        RenderableMesh* GetMeshContainingName(const std::string& namePart)
        {
            for (auto& mesh : entity->GetMeshes())
            {
                if (mesh->GetName().find(namePart) != std::string::npos)
                    return mesh;
            }
            return nullptr;
        }

        /**
         * @brief Adds a new script component of the given type to the entity.
         *
         * The script must derive from IScript. Memory is owned by the entity
         * after insertion.
         *
         * @tparam ClassScript The script class to instantiate.
         * @return Pointer to the created script instance.
         */
        template <typename ClassScript>
        ClassScript* AddComponent()
        {
            ClassScript* newScript = new ClassScript();
            entity->AddScript(newScript);
            return newScript;
        }

        /**
         * @brief Adds a script component already allocated externally.
         * @param script Script instance to attach.
         */
        void AddScript(IScript* script) { entity->AddScript(script); }

        /**
         * @brief Provides access to the material API for this entity.
         * @return Pointer to a MaterialApi instance.
         */
        MaterialApi* GetMaterialApi();

        /**
         * @brief Modifies the collider size of the entity's physics body.
         * @param size New size vector.
         */
        void SetColliderSize(const PulseEngine::Vector3& size);

        /**
         * @brief Sets the collider mass used by the physics system.
         * @param mass Mass value in kilograms.
         */
        void SetColliderMass(float mass);

        /**
         * @brief Assigns the physics body type of the collider.
         * @param pb Physics body identifier.
         */
        void SetColliderBody(int pb);

        /**
         * @brief Adds linear velocity to the physics body.
         * @param a Velocity vector to apply.
         */
        void AddVelocity(const PulseEngine::Vector3& a);

        /**
         * @brief Checks whether the current velocity magnitude is below a threshold.
         * @param value Threshold to compare against.
         * @return True if magnitude is lower, false otherwise.
         */
        bool IsVelocityLowerThan(float value);

        void AddAngularVelocity(const PulseEngine::Vector3& a);
    };
}

#endif
