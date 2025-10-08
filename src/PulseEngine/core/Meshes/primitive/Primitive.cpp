#include "Primitive.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/SceneLoader/SceneLoader.h"
#include <iostream>
#include <assimp/Importer.hpp>      // Assimp::Importer
#include <assimp/scene.h>           // aiScene
#include <assimp/postprocess.h>     // postprocessing flags

Mesh* Primitive::Cube()
{
    std::string path = "assets/model.fbx";

    Assimp::Importer* importer = new Assimp::Importer();

    //onced shared system implemented, we will use importer->ReadFileFromMemory() instead
    // it will be needed to have a method in our shared system that sent back a vector of char from the .pak
    //the importer will read the object from the .pak like this :
    //std::vector<char> fileData = LoadFromPak("assets/model.fbx");
    //Assimp::Importer importer;
    //
    //const aiScene* scene = importer.ReadFileFromMemory(
    //    fileData.data(),                   // pointer to memory
    //    fileData.size(),                   // size
    //    aiProcess_Triangulate |
    //    aiProcess_FlipUVs |
    //    aiProcess_GenNormals |
    //    aiProcess_CalcTangentSpace,
    //    "fbx"                              // file extension/hint
    //);

    const aiScene* scene = importer->ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Erreur Assimp: " << importer->GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Modèle chargé avec succès : " << path << std::endl;
    Mesh* msh = Mesh::LoadFromAssimp(scene->mMeshes[0], scene); // OK: importer toujours vivant ici

    msh->importer = importer;
    return msh;
}

Mesh *Primitive::Sphere()
{
    std::string path = "assets/model.fbx";

    Assimp::Importer* importer = new Assimp::Importer();

    //onced shared system implemented, we will use importer->ReadFileFromMemory() instead
    // it will be needed to have a method in our shared system that sent back a vector of char from the .pak
    //the importer will read the object from the .pak like this :
    //std::vector<char> fileData = LoadFromPak("assets/model.fbx");
    //Assimp::Importer importer;
    //
    //const aiScene* scene = importer.ReadFileFromMemory(
    //    fileData.data(),                   // pointer to memory
    //    fileData.size(),                   // size
    //    aiProcess_Triangulate |
    //    aiProcess_FlipUVs |
    //    aiProcess_GenNormals |
    //    aiProcess_CalcTangentSpace,
    //    "fbx"                              // file extension/hint
    //);

    const aiScene* scene = importer->ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Erreur Assimp: " << importer->GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Modèle chargé avec succès : " << path << std::endl;
    Mesh* msh = Mesh::LoadFromAssimp(scene->mMeshes[0], scene, nullptr); // OK: importer toujours vivant ici

    msh->importer = importer;
    return msh;
}
