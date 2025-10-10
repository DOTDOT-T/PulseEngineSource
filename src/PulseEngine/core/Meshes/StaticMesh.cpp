#include "StaticMesh.h"
#include "PulseEngine/core/Meshes/Mesh.h"

void StaticMesh::Update() 
{
}

void StaticMesh::Render(Shader *shader) const
{
    shader->SetBool("hasSkeleton", false);
    EDITOR_LOG("render a static mesh")
    for(Mesh* msh : meshes)
    {
        msh->Draw(shader);
    }
}