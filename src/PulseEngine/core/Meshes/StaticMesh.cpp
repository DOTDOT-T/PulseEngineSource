#include "StaticMesh.h"
#include "PulseEngine/core/Meshes/Mesh.h"

void StaticMesh::Update() 
{
}

void StaticMesh::Render(Shader *shader) const
{
    shader->SetBool("hasSkeleton", false);
    shader->SetInt("boneLength", 0);
    for(Mesh* msh : meshes)
    {
        msh->Draw(shader);
    }
}