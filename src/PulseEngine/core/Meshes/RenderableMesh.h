#ifndef RENDERABLEMESH_H
#define RENDERABLEMESH_H

#include "common/common.h"
#include "common/dllExport.h"

class Mesh;

class PULSE_ENGINE_DLL_API RenderableMesh
{
public:
    RenderableMesh(const std::string& name) : name(name) {}
    virtual void Update() = 0;
    virtual void Render(Shader* shader) const = 0;

    void AddMesh(Mesh* msh);

    void SetName(const std::string& name) {this->name = name;}
    std::string GetName() {return name;}
    
    std::size_t GetGuid() const { return guid; }
    void SetGuid(std::size_t newGuid) { guid = newGuid; }

    PulseEngine::Vector3 position = PulseEngine::Vector3(0.0f, 0.0f, 0.0f); ///< Position of the mesh in local space.
    PulseEngine::Vector3 rotation = PulseEngine::Vector3(0.0f, 0.0f, 0.0f); ///< Rotation of the mesh in local space.
    PulseEngine::Vector3 scale = PulseEngine::Vector3(1.0f, 1.0f, 1.0f); ///< Scale of the mesh in local space.
    PulseEngine::Mat4 matrix;

protected:
    std::vector<Mesh*> meshes;

private:
    std::string name;
    std::size_t guid;
    std::size_t muid = 0;

};


#endif