#ifndef SYNAPSE_H
#define SYNAPSE_H

#include "pch.h"

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/NodeMenuRegistry.h"
#include "PulseEngineEditor/InterfaceEditor/Synapse/Node.h"
#include <string>

class PULSE_ENGINE_DLL_API Synapse
{
public:
    Synapse(const std::string& synapsePath);
    ~Synapse() {}

    void Init();

    void Render();
    void RenderNodeMenu(const std::vector<NodeCategory>& categories);

    ed::EditorContext* context = nullptr;
};

#endif