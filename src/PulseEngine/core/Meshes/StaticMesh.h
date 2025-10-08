#ifndef STATICMESH_H
#define STATICMESH_H

#include "common/common.h"
#include "common/dllExport.h"
#include "PulseEngine/core/Meshes/RenderableMesh.h"

class PULSE_ENGINE_DLL_API StaticMesh : public RenderableMesh
{
public:
    StaticMesh(const std::string& name) : RenderableMesh(name) {}
    
    void Update() override;
    void Render(Shader* shader) const override;

private:
};

#endif