#include "RenderableMesh.h"
#include "PulseEngine/core/Meshes/Mesh.h"

void RenderableMesh::AddMesh(Mesh *msh)
{
    meshes.push_back(msh);
}