#include "Mesh.h"
#include "PulseEngine/core/Meshes/SkeletalMesh.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

Mesh::Mesh(const std::vector<float>& vertices)
{
    SetupMesh();
}

Mesh::Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices)
{
    SetupMesh();
}

Mesh::Mesh()
{  
}

Mesh::~Mesh()
{
    PulseEngineGraphicsAPI->DeleteMesh(&VAO, &VBO);
    // if (skeleton)
    // {
    //     delete skeleton;
    //     skeleton = nullptr;
    // }
    if(importer)
    {
        delete importer;
        importer = nullptr;
    }
}

void Mesh::SetupMesh()
{
    PulseEngineGraphicsAPI->SetupMesh(&VAO, &VBO, &EBO, vertices, indices);
}

void Mesh::Draw(Shader* shader)
{
    PulseEngineGraphicsAPI->RenderMesh(&VAO, &VBO, vertices, indices);
}

Mesh* Mesh::LoadFromAssimp(const aiMesh* mesh, const aiScene* scene, SkeletalMesh* skel)
{
    Mesh* newMesh = new Mesh();
    std::cout << "chargement du mesh" << std::endl;

    if (mesh->HasBones())
    {
        EDITOR_LOG("Bones founded")
        // Skeleton* skel = new Skeleton(mesh, scene);
        // newMesh->skeleton = skel;
    }

    try
    {
        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                newMesh->indices.push_back(face.mIndices[j]);
            }
        }
        std::cout << "face done" << std::endl;

        // Sommets
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex = {};

            // Position
            vertex.Position= PulseEngine::Vector3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            // Normales
            if (mesh->HasNormals())
            {
                vertex.Normal = PulseEngine::Vector3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }
            else
            {
                vertex.Normal = PulseEngine::Vector3(0.0f);
            }

            // UVs
            if (mesh->HasTextureCoords(0))
            {
                vertex.TexCoords = PulseEngine::Vector2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            }
            else
            {
                vertex.TexCoords = PulseEngine::Vector2(0.0f);
            }
            // === Compute Tangents & Bitangents ===
            if (mesh->HasTangentsAndBitangents())
            {
                vertex.Tangent = PulseEngine::Vector3(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                );
                vertex.Bitangent = PulseEngine::Vector3(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z
                );
            }
            else
            {
                vertex.Tangent = PulseEngine::Vector3(0.0f);
                vertex.Bitangent = PulseEngine::Vector3(0.0f);
            }


            // Initialisation à zéro pour les bones (sera rempli plus tard si bones)
            vertex.BoneIDs = PulseEngine::iVector4(0);
            vertex.Weights = PulseEngine::Vector4(0.0f);

            newMesh->vertices.push_back(vertex);
        }
        if (mesh->HasBones() && skel)
        {
            std::map<std::string, int> boneMapping;
            int boneCounter = 0;
        
            for (unsigned int i = 0; i < mesh->mNumBones; ++i)
            {
                std::string boneName(mesh->mBones[i]->mName.C_Str());
            
                int boneID = 0;
                if (skel->boneNameToIndex.find(boneName) == skel->boneNameToIndex.end())
                {
                    boneID = boneCounter;
                    boneCounter++;
                }
                else
                {
                    boneID = skel->boneNameToIndex[boneName];
                }
            
                const aiBone* bone = mesh->mBones[i];
                for (unsigned int j = 0; j < bone->mNumWeights; ++j)
                {
                    unsigned int vertexID = bone->mWeights[j].mVertexId;
                    float weight = bone->mWeights[j].mWeight;
                
                    if (vertexID < newMesh->vertices.size())
                    {
                        AddBoneDataToVertex(newMesh->vertices[vertexID], boneID, weight);
                    }
                }
            }
        for (Vertex &v : newMesh->vertices)
        {
            if (v.Weights.a == 0.0f && v.Weights.x == 0.0f && v.Weights.y == 0.0f && v.Weights.z == 0.0f)
            {
                v.BoneIDs[0] = 0;
                v.Weights[0] = 1.0f;
            }
        }
        }

        std::cout << "vertices construits ! " << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception dans le chargement des vertices: " << e.what() << std::endl;
        return nullptr;
    }

    std::cout << "chargement du mesh fini, passage au setup" << std::endl;
    newMesh->SetupMesh();
    std::cout << "setup du mesh fini" << std::endl;

    std::cout << "Nombre d'indices : " << newMesh->indices.size() << std::endl;

    unsigned int maxIndex = 0;
    for (unsigned int index : newMesh->indices)
    {
        if (index > maxIndex) maxIndex = index;
    }
    std::cout << "Indice max : " << maxIndex << ", Nombre de sommets : " << newMesh->vertices.size() << std::endl;
    std::cout << "mesh->HasFaces() ? " << mesh->HasFaces() << std::endl;
    std::cout << "mesh->mNumFaces : " << mesh->mNumFaces << std::endl;

    return newMesh;
}


void Mesh::UpdateMesh(float deltaTime)
{
    // if(skeleton)
    // {
    //     skeleton->UpdateSkeleton(deltaTime);
    // }
}

void Mesh::AddBoneDataToVertex(Vertex &vertex, int boneID, float weight)
{
    // Step 1: Add the bone
    int smallestWeightIndex = 0;
    float smallestWeight = vertex.Weights[0];

    // Find a slot with zero weight or the smallest weight
    for (int i = 0; i < 4; ++i)
    {
        if (vertex.Weights[i] == 0.0f)
        {
            vertex.BoneIDs[i] = boneID;
            vertex.Weights[i] = weight;
            goto normalize; // Early exit, normalize later
        }

        if (vertex.Weights[i] < smallestWeight)
        {
            smallestWeight = vertex.Weights[i];
            smallestWeightIndex = i;
        }
    }

    // Step 2: Replace the smallest weight if new weight is bigger
    if (weight > smallestWeight)
    {
        vertex.BoneIDs[smallestWeightIndex] = boneID;
        vertex.Weights[smallestWeightIndex] = weight;
    }

normalize:
    // Step 3: Normalize the weights so they sum to 1
    float sum = vertex.Weights[0] + vertex.Weights[1] + vertex.Weights[2] + vertex.Weights[3];
    if (sum > 0.0f)
    {
        vertex.Weights.a /= sum;
        vertex.Weights.x /= sum;
        vertex.Weights.y /= sum;
        vertex.Weights.z /= sum;
    }
}
