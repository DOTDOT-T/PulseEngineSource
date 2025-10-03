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
