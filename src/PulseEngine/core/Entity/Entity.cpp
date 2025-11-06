#include "Entity.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "shader.h"
#include "PulseEngine/core/Material/Material.h"
#include "Common/common.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

#include <algorithm>

PULSE_REGISTER_CLASS_CPP(Entity)


void Entity::Serialize(Archive &ar)
{
    
    ar.Serialize("name", name);
    transform.Serialize(ar);
    
}

void Entity::Deserialize(Archive &ar)
{
    EDITOR_LOG("deserialization is implemented")
}

const char* Entity::ToString()
{
    return std::string("EntityClass::" + name).c_str();
}

Entity::Entity(const std::string &name, const PulseEngine::Vector3 &position, Mesh *mesh, Material *material) : PulseObject(name.c_str()), material(material)
{
    //actually is it possible to be nullptr, because of loadScene, we need to load an entity with a material but no mesh before reading it.
    //so mesh can nul²lptr, and if it is pushed back into the meshes vector, it will cause a crash.
    // if(mesh) meshes.push_back(mesh);
    transform.position = position;
    transform.rotation = PulseEngine::Vector3(0.0f, 0.0f, 0.0f);
    transform.scale = PulseEngine::Vector3(1.0f, 1.0f, 1.0f);
    BaseConstructor();
}

Entity::Entity() : PulseObject()
{
    BaseConstructor();
}

void Entity::BaseConstructor()
{
    collider = new BoxCollider(&(this->transform.position), &(this->transform.rotation), PulseEngine::Vector3(1.0f, 1.0f, 1.0f));
    collider->owner = new PulseEngine::EntityApi(this);
    scripts.push_back(collider);
}

Entity::Entity(const std::string &name, const PulseEngine::Vector3 &position) : PulseObject(name.c_str())
{
    transform.position = position;
    BaseConstructor();
}

void Entity::UpdateModelMatrix(PulseEngine::Mat4 parentMatrix)
{
    this->entityMatrix = parentMatrix * transform.GetLocalMatrix();

    for(auto& mesh : meshes)
    {
        CalculateMeshMatrix(mesh);
    }
}


void Entity::SetMaterial(Material * material) { this->material = material; }

void Entity::UpdateEntity(PulseEngine::Mat4 parentMatrix)
{
    PROFILE_TIMER_FUNCTION;
    internalClock += PulseEngineInstance->GetDeltaTime();
    UpdateModelMatrix(parentMatrix);
    collider->SetRotation(transform.rotation);
    // IN_GAME_ONLY(
        for (size_t i = 0; i < scripts.size(); ++i)
        {
            scripts[i]->OnUpdate();
        }
    // )
    for (const auto &mesh : meshes)
    {
        mesh->Update();
    }

}

void Entity::DrawEntity() const
{
    PROFILE_TIMER_FUNCTION;
    material->GetShader()->SetMat4("model", GetMatrix());
    material->GetShader()->SetFloat("metallic", material ? material->specular : 1.0f);
    material->GetShader()->SetFloat("roughness", material ? material->roughness : 1.0f);
    material->GetShader()->SetVec3("objectColor", material ? material->color : PulseEngine::Vector3(0.5f));
    material->GetShader()->SetFloat("internalClock", internalClock);

    // Convert to radians
    float rx = PulseEngine::MathUtils::ToRadians(transform.rotation.x);
    float ry = PulseEngine::MathUtils::ToRadians(transform.rotation.y);
    float rz = PulseEngine::MathUtils::ToRadians(transform.rotation.z);

    // Build rotation matrices
    float cx = cos(rx), sx = sin(rx);
    float cy = cos(ry), sy = sin(ry);
    float cz = cos(rz), sz = sin(rz);

    // Rotation X
    float rotX[3][3] = {
        {1, 0, 0},
        {0, cx, -sx},
        {0, sx, cx}
    };

    // Rotation Y
    float rotY[3][3] = {
        {cy, 0, sy},
        {0, 1, 0},
        {-sy, 0, cy}
    };

    // Rotation Z
    float rotZ[3][3] = {
        {cz, -sz, 0},
        {sz, cz, 0},
        {0, 0, 1}
    };

    // Multiply Z * Y * X (rotation order you used)
    float rotZY[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            rotZY[i][j] = rotZ[i][0] * rotY[0][j] +
                          rotZ[i][1] * rotY[1][j] +
                          rotZ[i][2] * rotY[2][j];
        }
    }

    float rotMat[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            rotMat[i][j] = rotZY[i][0] * rotX[0][j] +
                           rotZY[i][1] * rotX[1][j] +
                           rotZY[i][2] * rotX[2][j];
        }
    }

    // Apply scale (multiply columns)
    rotMat[0][0] *= transform.scale.x; rotMat[1][0] *= transform.scale.x; rotMat[2][0] *= transform.scale.x;
    rotMat[0][1] *= transform.scale.y; rotMat[1][1] *= transform.scale.y; rotMat[2][1] *= transform.scale.y;
    rotMat[0][2] *= transform.scale.z; rotMat[1][2] *= transform.scale.z; rotMat[2][2] *= transform.scale.z;

    // Invert the 3×3
    float det = rotMat[0][0]*(rotMat[1][1]*rotMat[2][2] - rotMat[2][1]*rotMat[1][2]) -
                rotMat[0][1]*(rotMat[1][0]*rotMat[2][2] - rotMat[2][0]*rotMat[1][2]) +
                rotMat[0][2]*(rotMat[1][0]*rotMat[2][1] - rotMat[2][0]*rotMat[1][1]);

    float invDet = 1.0f / det;

    float invMat[3][3];
    invMat[0][0] =  (rotMat[1][1]*rotMat[2][2] - rotMat[2][1]*rotMat[1][2]) * invDet;
    invMat[0][1] = -(rotMat[0][1]*rotMat[2][2] - rotMat[2][1]*rotMat[0][2]) * invDet;
    invMat[0][2] =  (rotMat[0][1]*rotMat[1][2] - rotMat[1][1]*rotMat[0][2]) * invDet;

    invMat[1][0] = -(rotMat[1][0]*rotMat[2][2] - rotMat[2][0]*rotMat[1][2]) * invDet;
    invMat[1][1] =  (rotMat[0][0]*rotMat[2][2] - rotMat[2][0]*rotMat[0][2]) * invDet;
    invMat[1][2] = -(rotMat[0][0]*rotMat[1][2] - rotMat[1][0]*rotMat[0][2]) * invDet;

    invMat[2][0] =  (rotMat[1][0]*rotMat[2][1] - rotMat[2][0]*rotMat[1][1]) * invDet;
    invMat[2][1] = -(rotMat[0][0]*rotMat[2][1] - rotMat[2][0]*rotMat[0][1]) * invDet;
    invMat[2][2] =  (rotMat[0][0]*rotMat[1][1] - rotMat[1][0]*rotMat[0][1]) * invDet;

    // Transpose the inverse to get normal matrix
    PulseEngine::Mat3 normalMatrix;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            normalMatrix[i][j] = invMat[j][i];
        }
    }

    // Send to shader
    material->GetShader()->SetMat3("normalMatrix", normalMatrix);

    SimplyDrawMesh();
    // for(IScript* script : scripts)
    // {
    //     script->OnRender();
    // }
    // if(collider) collider->OnRender();
}

void Entity::UseShader() const
{
    material->GetShader()->Use();
}

void Entity::SimplyDrawMesh() const
{
    using namespace PulseEngine;

    for (RenderableMesh* mesh : meshes)
    {
        material->GetShader()->SetMat4("model", mesh->matrix);

        BindTexturesToShader();

        mesh->Render(material->GetShader());
    }


}

void Entity::CalculateMeshMatrix(RenderableMesh* const & mesh) const
{
    using namespace PulseEngine;
    Mat4 entityTransform = entityMatrix; // parent/world transform

    Mat4 localTransform = MathUtils::Matrix::Identity();
    localTransform = MathUtils::Matrix::Translate(localTransform, mesh->position);
    localTransform = MathUtils::Matrix::RotateZ(localTransform, MathUtils::ToRadians(mesh->rotation.z));
    localTransform = MathUtils::Matrix::RotateY(localTransform, MathUtils::ToRadians(mesh->rotation.y));
    localTransform = MathUtils::Matrix::RotateX(localTransform, MathUtils::ToRadians(mesh->rotation.x));
    localTransform = MathUtils::Matrix::Scale(localTransform, mesh->scale);

    mesh->matrix = entityTransform * localTransform;

}

void Entity::BindTexturesToShader() const
{
    if (auto albedoTex = material->GetTexture("albedo"))
    {
        albedoTex->Bind(6);
        material->GetShader()->SetInt("albedoMap", 6);
    }

    if (auto normalTex = material->GetTexture("normal"))
    {
        normalTex->Bind(7);
        material->GetShader()->SetInt("normalMap", 7);
    }
    if (auto normalTex = material->GetTexture("roughness"))
    {
        normalTex->Bind(8);
        material->GetShader()->SetInt("roughnessMap", 8);
    }
}

void Entity::DrawMeshWithShader(Shader* shader) const
{
    for (const auto &mesh : meshes)
    {        
        shader->SetMat4("model", mesh->matrix);
        mesh->Render(material->GetShader());
    }
}

void Entity::AddScript(IScript *script)
{
    if(!script) return;
    scripts.push_back(script);
    script->owner = new PulseEngine::EntityApi(this);
    script->OnStart();
}

void Entity::RemoveScript(IScript* script)
{
    auto it = std::find(scripts.begin(), scripts.end(), script);
    
    if (it != scripts.end()) 
    {
        scripts.erase(it);
    }
}

void Entity::AddTag(const std::string &tag)
{
    if (std::find(tags.begin(), tags.end(), tag) == tags.end())
    {
        tags.push_back(tag);
    }
}

void Entity::RemoveTag(const std::string &tag)
{
    auto it = std::remove(tags.begin(), tags.end(), tag);
    if (it != tags.end())
    {
        tags.erase(it);
    }
}

bool Entity::HasTag(const std::string & tag) const
{
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

void Entity::Move(const PulseEngine::Vector3 &direction)
{
    transform.position = transform.position + (direction * PulseEngineInstance->GetDeltaTime());
}

void Entity::Rotate(const PulseEngine::Vector3 &rotation)
{
    this->transform.rotation = this->transform.rotation + (rotation * PulseEngineInstance->GetDeltaTime());
}
