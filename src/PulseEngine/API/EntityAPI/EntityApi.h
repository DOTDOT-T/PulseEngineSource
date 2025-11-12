#ifndef ENTITY_API_H
#define ENTITY_API_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/CustomScripts/IScripts.h"

class MaterialApi;

namespace PulseEngine
{
    class PULSE_ENGINE_DLL_API EntityApi
    {
        private:
            Entity* entity;
            MaterialApi* materialApi;
        public:
            EntityApi(Entity* e);
    
            PulseEngine::Vector3 GetPosition() {return entity->GetPosition(); }
            PulseEngine::Vector3 GetRotation() {return entity->GetRotation(); }
            void SetPosition(const PulseEngine::Vector3& pos) {entity->SetPosition(pos);}
            void SetRotation(const PulseEngine::Vector3& rot) {entity->SetRotation(rot);}
            void Rotate(const PulseEngine::Vector3& rot) { entity->SetRotation(entity->GetRotation() + rot);}
            void Move(const PulseEngine::Vector3& move) {entity->transform.position += move;}

            Transform* GetTransform() { return &entity->transform; }
    
            /**
             * @brief Get a specific component from the entity.
             * The component wanted need to be a derivated class from "IScript"
             * if not, it will never be possible to get a specific component.
             * 
             * @return T* 
             */
            template <typename ClassScript>
            ClassScript* GetComponent() 
            {
                for (auto* script : entity->GetScripts()) 
                {
                    if (auto* ptr = dynamic_cast<ClassScript*>(script)) 
                    {
                        return ptr;
                    }
                }
                return nullptr;
            }

            RenderableMesh* GetMeshByName(const std::string& name)
            {
                for (auto& mesh : entity->GetMeshes()) 
                {
                    if (mesh->GetName() == name) 
                    {
                        return mesh;
                    }
                }
                return nullptr;
            }

            std::vector<RenderableMesh*> GetMeshesContainingName(const std::string& namePart)
            {
                std::vector<RenderableMesh*> matchingMeshes;
                for (auto& mesh : entity->GetMeshes()) 
                {
                    if (mesh->GetName().find(namePart) != std::string::npos) 
                    {
                        matchingMeshes.push_back(mesh);
                    }
                }
                return matchingMeshes;
            }

            RenderableMesh* GetMeshContainingName(const std::string& namePart)
            {
                std::vector<RenderableMesh*> matchingMeshes;
                for (auto& mesh : entity->GetMeshes()) 
                {
                    if (mesh->GetName().find(namePart) != std::string::npos) 
                    {
                        return mesh;
                    }
                }
                return nullptr;
            }

        
            /**
             * @brief To easily add a component into the entity. 
             * @note ClassScript need to be a derivated class from "IScript", if not, the component will not be added to the entity.
             * 
             * @tparam ClassScript the component class name you want to add.
             * @return ClassScript* the generated component that is added to the entity, to easily access it.
             */
            template <typename ClassScript>
            ClassScript* AddComponent()
            {
                ClassScript* newScript = new ClassScript();
                entity->AddScript(newScript);
            
                return newScript;
            }
        
            void AddScript(IScript* script) {entity->AddScript(script); }

            MaterialApi* GetMaterialApi();
        
    };
}

#endif
