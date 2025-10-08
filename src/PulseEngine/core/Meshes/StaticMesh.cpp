#include "StaticMesh.h"
#include "PulseEngine/core/Meshes/Mesh.h"

void StaticMesh::Update() 
{
}

void StaticMesh::Render(Shader *shader) const
{
    for(Mesh* msh : meshes)
    {
        msh->Draw(shader);
    }
}