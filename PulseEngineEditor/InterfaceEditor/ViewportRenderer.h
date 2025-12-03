#ifndef __VIEWPORTRENDERER_H__
#define __VIEWPORTRENDERER_H__

#include "Common/common.h"
#include "Common/dllExport.h"

#include  "PulseEngine/ModuleLoader/IModuleInterface/IModuleInterface.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"

class PULSE_ENGINE_DLL_API Viewport : public IModuleInterface
{
    public:
        std::string GetName() const override;

        std::string GetVersion() const override;

        void Initialize() override;

        void Shutdown() override;

        void Render() override;
        void SceneRenderer();

    private:
        PulseEngine::Vector2 previewData;
        TransformOperator top = TransformOperator::TRANSLATE;
};


#endif // __VIEWPORTRENDERER_H__