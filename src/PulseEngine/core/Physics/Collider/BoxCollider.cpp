#include "BoxCollider.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Meshes/Vertex.h"

#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>

BoxCollider::BoxCollider(PulseEngine::Vector3* position, PulseEngine::Vector3* rotation, const PulseEngine::Vector3& size)
    : Collider(), position(position), rotation(rotation), size(size)
{
    hasFastCalculus = false;
    AddExposedVariable(EXPOSE_VAR(isTrigger, BOOL));
    REGISTER_VAR(isTrigger);
    AddExposedVariable(EXPOSE_VAR(boxColor, FLOAT3));
    REGISTER_VAR(boxColor);
    AddExposedVariable(EXPOSE_VAR(this->size, FLOAT3));
    REGISTER_VAR(this->size);
    // AddExposedVariable(EXPOSE_VAR(decalPosition.x, FLOAT3));
    // REGISTER_VAR(decalPosition.x);

    IGraphicsAPI* gAPI = PulseEngineInstance->graphicsAPI;
    if (!gAPI) return;

    // Calcul du half-size
    PulseEngine::Vector3 halfSize = GetHalfSize();

    // Définition des vertices du cube
    PulseEngine::Vector3 vertices[8] = {
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f}
    };

    
    unsigned int indices[24] = {
        0,1, 1,2, 2,3, 3,0, 
        4,5, 5,6, 6,7, 7,4, 
        0,4, 1,5, 2,6, 3,7  
    };

    meshVertices.clear();
    meshVertices.reserve(8);
    for (auto v : vertices)
    {
        Vertex vert;
        vert.Position = v;
        vert.Normal = PulseEngine::Vector3(0.0f);
        vert.TexCoords = PulseEngine::Vector2(0.0f);
        vert.BoneIDs = PulseEngine::iVector4(0);
        vert.Weights = PulseEngine::Vector4(0.0f);
        vert.Tangent = PulseEngine::Vector3(0.0f);
        vert.Bitangent = PulseEngine::Vector3(0.0f);
        meshVertices.push_back(vert);
    }

    // Indices
    meshIndices = std::vector<unsigned int>(std::begin(indices), std::end(indices));

    // Création du mesh OpenGL
    gAPI->SetupMesh(&VAO, &VBO, &EBO, meshVertices, meshIndices);

    // Chargement du shader de ligne
    lineTraceShader = new Shader(
        std::string(ASSET_PATH) + "shaders/lineTrace.vert",
        std::string(ASSET_PATH) + "shaders/lineTrace.frag",
        gAPI
    );


}

void BoxCollider::OnUpdate()
{
    boxColor = othersCollider.size() > 0 ? PulseEngine::Color(0.0f,255.0f,0.0f) : PulseEngine::Color(255.0f, 0.0f, 0.0f);

    if (physicBody == PhysicBody::STATIC)
        return;

    PulseEngine::Vector3 gravity(0.0f, -9.81f, 0.0f);
    PulseEngine::Vector3 totalForce = force + (mass * gravity);

    PulseEngine::Vector3 acceleration = totalForce / mass;

    velocity += acceleration * PulseEngineInstance->GetDeltaTime();


    velocity *= (1.0f - std::min(0.99f, 0.05f * othersCollider.size()) * PulseEngineInstance->GetDeltaTime()); 

    SetPosition(GetPosition() + velocity * PulseEngineInstance->GetDeltaTime());

    force = PulseEngine::Vector3(0, 0, 0);
    


}

void BoxCollider::OnRender()
{
    IGraphicsAPI* gAPI = PulseEngineInstance->graphicsAPI;
    if (!gAPI) return;

    gAPI->ActivateWireframe();

    PulseEngine::Mat4 model = PulseEngine::MathUtils::Matrix::Identity();
    model = PulseEngine::MathUtils::Matrix::Translate(model, (*position));
    model = PulseEngine::MathUtils::Matrix::RotateZ(model, PulseEngine::MathUtils::ToRadians(rotation->z));
    model = PulseEngine::MathUtils::Matrix::RotateY(model, PulseEngine::MathUtils::ToRadians(rotation->y));
    model = PulseEngine::MathUtils::Matrix::RotateX(model, PulseEngine::MathUtils::ToRadians(rotation->x));
    model = PulseEngine::MathUtils::Matrix::Scale(model, size);

    // lineTraceShader->Use();
    // gAPI->SetShaderMat4(lineTraceShader, "view", PulseEngineInstance->view); 
    // gAPI->SetShaderMat4(lineTraceShader, "projection", PulseEngineInstance->projection);
    gAPI->SetShaderMat4(lineTraceShader, "model", model); 
    gAPI->SetShaderVec3(lineTraceShader, "color", PulseEngine::Vector3(boxColor.r/255.0f,boxColor.g/255.0f,boxColor.b/255.0f)); 
    std::vector<PulseEngine::Vector3> mshVertPos;
    for (int i = 0; i < 8; ++i)
    {
        PulseEngine::Vector3 v = meshVertices[i].Position;
        mshVertPos.push_back(v); 
    }
    gAPI->RenderLineMesh(&VAO, &VBO, mshVertPos, meshIndices);

    gAPI->DesactivateWireframe();
}

void BoxCollider::OnEditorDisplay()
{
}

PulseEngine::Vector3 BoxCollider::GetOrientedSize(const PulseEngine::Vector3& rotation, const PulseEngine::Vector3& originalSize)
{
    using namespace std;
    // Convertit les rotations en radians
    float rotX = rotation.x * (M_PI / 180.0f);
    float rotY = rotation.y * (M_PI / 180.0f);
    float rotZ = rotation.z * (M_PI / 180.0f);

    // Approximation basique : calcule la "bounding box" agrandie à cause de la rotation
    float cosX = std::abs(cos(rotX));
    float sinX = std::abs(sin(rotX));
    float cosY = std::abs(cos(rotY));
    float sinY = std::abs(sin(rotY));
    float cosZ = std::abs(cos(rotZ));
    float sinZ = std::abs(sin(rotZ));

    PulseEngine::Vector3 orientedSize;
    orientedSize.x = originalSize.x * cosY * cosZ + originalSize.y * sinZ + originalSize.z * sinY;
    orientedSize.y = originalSize.y * cosX * cosZ + originalSize.x * sinZ + originalSize.z * sinX;
    orientedSize.z = originalSize.z * cosX * cosY + originalSize.x * sinY + originalSize.y * sinX;

    return orientedSize;
}



bool BoxCollider::CheckCollision(Collider* other)
{
    auto* otherBox = dynamic_cast<BoxCollider*>(other);
    if (!otherBox)
    {
        return false; // Types incompatibles pour l’instant
    }
    if(this->hasFastCalculus) return FastCheckCollision(otherBox);

    return SeparatedAxisDetection(otherBox);
}

#pragma region For BOX collision detection

bool BoxCollider::SeparatedAxisDetection(BoxCollider* otherBox)
{
    PulseEngine::Vector3 centerA = this->GetCenter();
    PulseEngine::Vector3 centerB = otherBox->GetCenter();

    PulseEngine::Vector3 axisA[3] = { GetAxis(0), GetAxis(1), GetAxis(2) };
    PulseEngine::Vector3 axisB[3] = { otherBox->GetAxis(0), otherBox->GetAxis(1), otherBox->GetAxis(2) };

    float ra, rb;
    PulseEngine::Mat3 R, AbsR;

    // Compute rotation matrix expressing boxB in boxA's coordinate frame
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[i][j] = PulseEngine::Dot(axisA[i], axisB[j]);
            AbsR[i][j] = std::abs(R[i][j]) + 1e-5f; // epsilon to handle precision
        }
    }

    // Vector from A to B in A's local coordinates
    PulseEngine::Vector3 t(0.0f, 0.0f, 0.0f);
    t.x = centerB.x - centerA.x;
    t.y = centerB.y - centerA.y;
    t.z = centerB.z - centerA.z;
    t = PulseEngine::Vector3(PulseEngine::Dot(t, axisA[0]), PulseEngine::Dot(t, axisA[1]), PulseEngine::Dot(t, axisA[2]));

    PulseEngine::Vector3 halfA = this->GetHalfSize();
    PulseEngine::Vector3 halfB = otherBox->GetHalfSize();

    // Test axes A0, A1, A2
    for (int i = 0; i < 3; ++i)
    {
        ra = halfA[i];
        rb = halfB[0] * AbsR[i][0] + halfB[1] * AbsR[i][1] + halfB[2] * AbsR[i][2];
        if (std::abs(t[i]) > ra + rb) return false;
    }

    // Test axes B0, B1, B2
    for (int i = 0; i < 3; ++i)
    {
        ra = halfA[0] * AbsR[0][i] + halfA[1] * AbsR[1][i] + halfA[2] * AbsR[2][i];
        rb = halfB[i];
        if (std::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            // Compute the axis
            PulseEngine::Vector3 axis = PulseEngine::Cross(axisA[i], axisB[j]);

            // Ignore near-zero vectors (i.e. parallel axes)
            if (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z < 1e-6f)
                continue; // Skip degenerate axis

            axis = PulseEngine::Normalize(axis);

            // Project t onto this axis
            float tProj = std::abs(PulseEngine::Dot(t, axis));

            // Project both boxes onto the axis
            ra = halfA[0] * std::abs(PulseEngine::Dot(axis, axisA[0])) +
                 halfA[1] * std::abs(PulseEngine::Dot(axis, axisA[1])) +
                 halfA[2] * std::abs(PulseEngine::Dot(axis, axisA[2]));

            rb = halfB[0] * std::abs(PulseEngine::Dot(axis, axisB[0])) +
                 halfB[1] * std::abs(PulseEngine::Dot(axis, axisB[1])) +
                 halfB[2] * std::abs(PulseEngine::Dot(axis, axisB[2]));

            if (tProj > ra + rb)
                return false; // Separation axis found
        }
    }
    return true; // No separation axis found
}

bool BoxCollider::FastCheckCollision(BoxCollider *otherBox)
{
    PulseEngine::Vector3 posA = this->GetPosition();
    PulseEngine::Vector3 sizeA = this->GetSize();

    PulseEngine::Vector3 posB = otherBox->GetPosition();
    PulseEngine::Vector3 sizeB = otherBox->GetSize();

    sizeA.x /= 2.0f;
    sizeA.y /= 2.0f;
    sizeA.z /= 2.0f;
    sizeB.x /= 2.0f;
    sizeB.y /= 2.0f;
    sizeB.z /= 2.0f;


    bool xOverlap = std::abs(posA.x - posB.x) <= (sizeA.x + sizeB.x);
    bool yOverlap = std::abs(posA.y - posB.y) <= (sizeA.y + sizeB.y);
    bool zOverlap = std::abs(posA.z - posB.z) <= (sizeA.z + sizeB.z);

    bool collisionDetected = xOverlap && yOverlap && zOverlap;

    return collisionDetected;
}

#pragma endregion

void BoxCollider::ResolveCollision(Collider* other)
{
    auto* otherBox = dynamic_cast<BoxCollider*>(other);
    if (!otherBox)
    {
        return;
    }

    PulseEngine::Vector3 posA = this->GetPosition();
    PulseEngine::Vector3 sizeA = GetOrientedSize(*this->rotation, this->GetSize());
    PulseEngine::Vector3 posB = otherBox->GetPosition();
    PulseEngine::Vector3 sizeB = GetOrientedSize(*otherBox->rotation, otherBox->GetSize());

    if(other->physicBody == PhysicBody::MOVABLE && this->physicBody == PhysicBody::MOVABLE)
    {
        PulseEngine::Vector3 normal = (other->GetPosition() - GetPosition()).Normalized();
        float relativeVel = (other->velocity - velocity).Dot(normal);

        if (relativeVel > 0.0f)
            return; 

        float e = 0.5f;
        float j = -(1 + e) * relativeVel / (1 / mass + 1 / other->mass);

        PulseEngine::Vector3 impulse = j * normal;

        velocity -= impulse / mass;
        other->velocity += impulse / other->mass;
    }
    else
    {
        sizeA.x /= 2.0f;
        sizeA.y /= 2.0f;
        sizeA.z /= 2.0f;
        sizeB.x /= 2.0f;
        sizeB.y /= 2.0f;
        sizeB.z /= 2.0f;

        float deltaX = posB.x - posA.x;
        float intersectX = (sizeA.x + sizeB.x) - std::abs(deltaX);

        float deltaY = posB.y - posA.y;
        float intersectY = (sizeA.y + sizeB.y) - std::abs(deltaY);

        float deltaZ = posB.z - posA.z;
        float intersectZ = (sizeA.z + sizeB.z) - std::abs(deltaZ);

        // Trouve le plus petit axe d'intersection
        if (intersectX < intersectY && intersectX < intersectZ)
        {
            float pushX = (deltaX > 0) ? -intersectX : intersectX;
            posA.x += pushX;
        }
        else if (intersectY < intersectZ)
        {
            float pushY = (deltaY > 0) ? -intersectY : intersectY;
            posA.y += pushY;
            velocity.y = 0.0f;
        }
        else
        {
            float pushZ = (deltaZ > 0) ? -intersectZ : intersectZ;
            posA.z += pushZ;
        }

        this->SetPosition(posA);
    }
}

PulseEngine::Vector3 BoxCollider::GetCenter() const
{
    return *position;
}

PulseEngine::Vector3 BoxCollider::GetHalfSize() const
{
    return PulseEngine::Vector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
}

PulseEngine::Vector3 BoxCollider::GetAxis(int index) const
{
    // Convertir les angles en radians
    float radX = rotation->x * (M_PI / 180.0f);
    float radY = rotation->y * (M_PI / 180.0f);
    float radZ = rotation->z * (M_PI / 180.0f);

    // Matrices de rotation basiques
    PulseEngine::Mat3 rotX = PulseEngine::Mat3::RotationX(radX);
    PulseEngine::Mat3 rotY = PulseEngine::Mat3::RotationY(radY);
    PulseEngine::Mat3 rotZ = PulseEngine::Mat3::RotationZ(radZ);

    // Rotation finale : Y * X * Z
    PulseEngine::Mat3 rotationMatrix = rotZ * rotY * rotX;



    // Axes locaux de base (avant rotation)
    switch (index)
    {
        case 0: return rotationMatrix * PulseEngine::Vector3(1, 0, 0); // X
        case 1: return rotationMatrix * PulseEngine::Vector3(0, 1, 0); // Y
        case 2: return rotationMatrix * PulseEngine::Vector3(0, 0, 1); // Z
        default: return PulseEngine::Vector3(0, 0, 0); // erreur
    }
}

