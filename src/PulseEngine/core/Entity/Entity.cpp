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

#include <algorithm>

Entity::Entity(const std::string &name, const PulseEngine::Vector3 &position, Mesh* mesh, Material* material) :name(name), position(position), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), material(material)
{
    //actually is it possible to be nullptr, because of loadScene, we need to load an entity with a material but no mesh before reading it.
    //so mesh can nullptr, and if it is pushed back into the meshes vector, it will cause a crash.
    if(mesh) meshes.push_back(mesh);

    BaseConstructor();
}

void Entity::BaseConstructor()
{
    UpdateModelMatrix();
    collider = new BoxCollider(&this->position, &this->rotation, PulseEngine::Vector3(1.0f, 1.0f, 1.0f));
    collider->owner = new PulseEngine::EntityApi(this);
}

Entity::Entity(const std::string &name, const PulseEngine::Vector3 &position) : name(name), position(position)
{
    BaseConstructor();
}

void Entity::UpdateModelMatrix()
{
    using namespace PulseEngine;

    Mat4 localMat = PulseEngine::MathUtils::Matrix::Identity();
    localMat = PulseEngine::MathUtils::Matrix::Translate(localMat, position);
    localMat = PulseEngine::MathUtils::Matrix::RotateZ(localMat, PulseEngine::MathUtils::ToRadians(rotation.z));
    localMat = PulseEngine::MathUtils::Matrix::RotateY(localMat, PulseEngine::MathUtils::ToRadians(rotation.y));
    localMat = PulseEngine::MathUtils::Matrix::RotateX(localMat, PulseEngine::MathUtils::ToRadians(rotation.x));
    localMat = PulseEngine::MathUtils::Matrix::Scale(localMat, scale);

    this->entityMatrix = localMat;

    for(auto& mesh : meshes)
    {
        CalculateMeshMatrix(mesh);
    }
}


void Entity::SetMaterial(Material * material) { this->material = material; }

void Entity::UpdateEntity(float deltaTime)
{
    internalClock += deltaTime;
    UpdateModelMatrix();
    collider->SetRotation(rotation);
    IN_GAME_ONLY(
        for (size_t i = 0; i < scripts.size(); ++i)
        {
            scripts[i]->OnUpdate();
        }
    )
    for (const auto &mesh : meshes)
    {
        mesh->UpdateMesh(deltaTime);
    }

}

void Entity::DrawEntity() const
{
    material->GetShader()->SetMat4("model", GetMatrix());
    material->GetShader()->SetFloat("metallic", material ? material->specular : 1.0f);
    material->GetShader()->SetFloat("roughness", material ? material->roughness : 1.0f);
    material->GetShader()->SetVec3("objectColor", material ? material->color : PulseEngine::Vector3(0.5f));
    material->GetShader()->SetFloat("internalClock", internalClock);

    // Convert to radians
    float rx = PulseEngine::MathUtils::ToRadians(rotation.x);
    float ry = PulseEngine::MathUtils::ToRadians(rotation.y);
    float rz = PulseEngine::MathUtils::ToRadians(rotation.z);

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
    rotMat[0][0] *= scale.x; rotMat[1][0] *= scale.x; rotMat[2][0] *= scale.x;
    rotMat[0][1] *= scale.y; rotMat[1][1] *= scale.y; rotMat[2][1] *= scale.y;
    rotMat[0][2] *= scale.z; rotMat[1][2] *= scale.z; rotMat[2][2] *= scale.z;

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
}

void Entity::UseShader() const
{
    material->GetShader()->Use();
}

void Entity::SimplyDrawMesh() const
{
    using namespace PulseEngine;

    for (const auto& mesh : meshes)
    {
        
        material->GetShader()->SetMat4("model", entityMatrix);

        BindTexturesToShader();

        mesh->Draw(material->GetShader()->getProgramID());
    }
}

void Entity::CalculateMeshMatrix(Mesh *const & mesh) const
{
    using namespace PulseEngine;
    Mat4 entityTransform = entityMatrix; // parent/world transform

    Mat4 localTransform = MathUtils::Matrix::Identity();
    localTransform = MathUtils::Matrix::Translate(localTransform, mesh->position);
    localTransform = MathUtils::Matrix::RotateX(localTransform, MathUtils::ToRadians(mesh->rotation.x));
    localTransform = MathUtils::Matrix::RotateY(localTransform, MathUtils::ToRadians(mesh->rotation.y));
    localTransform = MathUtils::Matrix::RotateZ(localTransform, MathUtils::ToRadians(mesh->rotation.z));
    localTransform = MathUtils::Matrix::Scale(localTransform, mesh->scale);

    // Correct order: local × parent
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

void Entity::DrawMeshWithShader(unsigned int shaderProgram) const
{
    for (const auto &mesh : meshes)
    {        
        material->GetShader()->SetMat4("model", mesh->matrix);
        mesh->Draw(shaderProgram);
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
    position = position + (direction * PulseEngineInstance->GetDeltaTime());
}

void Entity::Rotate(const PulseEngine::Vector3 &rotation)
{
    this->rotation = this->rotation + (rotation * PulseEngineInstance->GetDeltaTime());
}
