#include "BoxCollider.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Meshes/Vertex.h"
#include "PulseEngine/API/EngineApi.h"

#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>

#define M_PI 3.14159265358979323846

#include "PulseEngine/core/FileManager/Archive/Archive.h"

// PULSE_REGISTER_CLASS_CPP(BoxCollider)


// void BoxCollider::Serialize(Archive& ar)
// {
//     ar.Serialize("physicType", physicBody);
//     ar.Serialize("mass", mass);
//     ar.Serialize("size.x", size.x);
//     ar.Serialize("size.y", size.y);
//     ar.Serialize("size.z", size.z);
// }

// void BoxCollider::Deserialize(Archive& ar)
// {

// }
// const char* BoxCollider::ToString()
// {
//     return "BoxCollider";
// }



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

BoxCollider::BoxCollider()
{
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


    // velocity *= (1.0f - std::min(0.99f, 0.05f + 0.85f * othersCollider.size()) * PulseEngineInstance->GetDeltaTime()); 

    if (othersCollider.size() > 0)
    {
        PulseEngine::Vector3 normal(0, 1, 0);

        float normalVel = velocity.Dot(normal);
        if (normalVel < 0)
            velocity -= normal * normalVel;
    
        PulseEngine::Vector3 tangent = velocity - normal * velocity.Dot(normal);
        float tangentLen = tangent.GetMagnitude();

        if (tangentLen > 0.0001f)
        {
            tangent.Normalized();

            float mu = 0.8f;
            float frictionMag = mu * 9.81f * PulseEngineInstance->GetDeltaTime();

            frictionMag = std::min(frictionMag, tangentLen);

            velocity -= tangent * frictionMag;
        }
    }
    velocity -= (velocity *0.1)*PulseEngineInstance->GetDeltaTime();
    PulseEngine::Vector3 localInertia = PulseEngine::Vector3(
    1.0f / 12.0f * mass * (size.y*size.y + size.z*size.z),
    1.0f / 12.0f * mass * (size.x*size.x + size.z*size.z),
    1.0f / 12.0f * mass * (size.x*size.x + size.y*size.y)
    );

    angularVelocity.x -= angularVelocity.x * PulseEngineInstance->GetDeltaTime() * 0.5f / localInertia.x;
    angularVelocity.y -= angularVelocity.y * PulseEngineInstance->GetDeltaTime() * 0.5f / localInertia.y;
    angularVelocity.z -= angularVelocity.z * PulseEngineInstance->GetDeltaTime() * 0.5f / localInertia.z;

    SetPosition(GetPosition() + velocity * PulseEngineInstance->GetDeltaTime());


    owner->GetTransform()->AddWorldRotation(angularVelocity * (180.0f / M_PI) * PulseEngineInstance->GetDeltaTime());

    force = PulseEngine::Vector3(0, 0, 0);
}

void BoxCollider::OnRender()
{
    IGraphicsAPI* gAPI = PulseEngineInstance->graphicsAPI;
    if (!gAPI) return;

    gAPI->ActivateWireframe();

    PulseEngine::Mat4 model = PulseEngine::MathUtils::Matrix::Identity();
    model = PulseEngine::MathUtils::Matrix::Translate(model, (*position) + decalPosition);
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
    auto MakeAxes = [&](const PulseEngine::Vector3& rot) {
        float radX = rot.x * (M_PI / 180.0f);
        float radY = rot.y * (M_PI / 180.0f);
        float radZ = rot.z * (M_PI / 180.0f);
        PulseEngine::Mat3 rx = PulseEngine::Mat3::RotationX(radX);
        PulseEngine::Mat3 ry = PulseEngine::Mat3::RotationY(radY);
        PulseEngine::Mat3 rz = PulseEngine::Mat3::RotationZ(radZ);
        PulseEngine::Mat3 R = rx * ry * rz;
        PulseEngine::Vector3 a0 = R * PulseEngine::Vector3(1,0,0);
        PulseEngine::Vector3 a1 = R * PulseEngine::Vector3(0,1,0);
        PulseEngine::Vector3 a2 = R * PulseEngine::Vector3(0,0,1);
        return std::array<PulseEngine::Vector3,3>{a0,a1,a2};
    };

    auto axes = MakeAxes(rotation);

    PulseEngine::Vector3 h = PulseEngine::Vector3(originalSize.x * 0.5f, originalSize.y * 0.5f, originalSize.z * 0.5f);

    PulseEngine::Vector3 halfExtentsWorld;
    halfExtentsWorld.x = std::abs(axes[0].x) * h.x + std::abs(axes[1].x) * h.y + std::abs(axes[2].x) * h.z;
    halfExtentsWorld.y = std::abs(axes[0].y) * h.x + std::abs(axes[1].y) * h.y + std::abs(axes[2].y) * h.z;
    halfExtentsWorld.z = std::abs(axes[0].z) * h.x + std::abs(axes[1].z) * h.y + std::abs(axes[2].z) * h.z;

    return PulseEngine::Vector3(halfExtentsWorld.x * 2.0f, halfExtentsWorld.y * 2.0f, halfExtentsWorld.z * 2.0f);
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

bool BoxCollider::CheckPositionCollision(const PulseEngine::Vector3& pos)
{
    // Convert world point to local OBB space

    PulseEngine::Mat4 rotationMatrix = PulseEngine::MathUtils::Matrix::Identity();
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateZ(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->z));
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateY(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->y));
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateX(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->x));

    PulseEngine::Vector3 local = PulseEngine::MathUtils::Matrix::Transpose(rotationMatrix) * (pos - *position);

    return std::abs(local.x) <= size.x * 0.5f &&
           std::abs(local.y) <= size.y * 0.5f &&
           std::abs(local.z) <= size.z * 0.5f;
}

#pragma region For BOX collision detection

bool BoxCollider::SeparatedAxisDetection(BoxCollider* otherBox)
{
    PulseEngine::Vector3 axis;
    float depth;
    bool coll = SAT_MinimumTranslation(*otherBox, axis, depth);
    return coll;
}

bool BoxCollider::FastCheckCollision(BoxCollider* otherBox)
{
    PulseEngine::Vector3 posA = this->GetCenter() + decalPosition;
    PulseEngine::Vector3 posB = otherBox->GetCenter() + otherBox->decalPosition;

    PulseEngine::Vector3 halfA = GetHalfSize();
    PulseEngine::Vector3 halfB = otherBox->GetHalfSize();

    // A's axes in world space
    PulseEngine::Vector3 axesA[3] = { GetAxis(0), GetAxis(1), GetAxis(2) };
    // B's axes in world space
    PulseEngine::Vector3 axesB[3] = { otherBox->GetAxis(0), otherBox->GetAxis(1), otherBox->GetAxis(2) };

    // Vector from A to B
    PulseEngine::Vector3 t = posB - posA;

    // Check separation along A's axes
    for (int i = 0; i < 3; ++i)
    {
        float ra = halfA[i];
        float rb = halfB.x * std::abs(PulseEngine::Dot(axesA[i], axesB[0])) +
                   halfB.y * std::abs(PulseEngine::Dot(axesA[i], axesB[1])) +
                   halfB.z * std::abs(PulseEngine::Dot(axesA[i], axesB[2]));

        if (std::abs(PulseEngine::Dot(t, axesA[i])) > ra + rb)
            return false;
    }

    // Check separation along B's axes
    for (int i = 0; i < 3; ++i)
    {
        float ra = halfA.x * std::abs(PulseEngine::Dot(axesB[i], axesA[0])) +
                   halfA.y * std::abs(PulseEngine::Dot(axesB[i], axesA[1])) +
                   halfA.z * std::abs(PulseEngine::Dot(axesB[i], axesA[2]));

        float rb = halfB[i];

        if (std::abs(PulseEngine::Dot(t, axesB[i])) > ra + rb)
            return false;
    }

    // If we reach here, no separating axis found -> collision
    return true;
}


#pragma endregion

void BoxCollider::ResolveCollision(Collider* other)
{
    auto* otherBox = dynamic_cast<BoxCollider*>(other);
    if (!otherBox || this->isTrigger || otherBox->isTrigger) return;

    // --- Phase 1 : SAT collision check ---
    PulseEngine::Vector3 normal;
    float penetration;
    if (!SAT_MinimumTranslation(*otherBox, normal, penetration))
        return;

    PulseEngine::Vector3 posA = this->GetCenter() + decalPosition;
    PulseEngine::Vector3 posB = otherBox->GetCenter() + otherBox->decalPosition;

    // S'assurer que la normale pointe de A vers B
    if ((posB - posA).Dot(normal) < 0.0f)
        normal = PulseEngine::Vector3(-normal.x, -normal.y, -normal.z);

    // --- Phase 2 : désenfouissement ---

    const float slop = 0.001f;
    float invMassA = (physicBody == PhysicBody::MOVABLE) ? 1.0f / mass : 0.0f;
    float invMassB = (otherBox->physicBody == PhysicBody::MOVABLE) ? 1.0f / otherBox->mass : 0.0f;
    float percent;
    if (invMassA == 0.0f || invMassB == 0.0f)
        percent = 1.0f;     // full correction for interaction with statique
    else
        percent = 0.5f;
    float correctionMag = std::max(penetration - slop, 0.0f) / (invMassA + invMassB) * percent;
    PulseEngine::Vector3 correction = correctionMag * normal;

    if (physicBody == PhysicBody::MOVABLE && otherBox->physicBody == PhysicBody::STATIC)
    {
        SetPosition(GetPosition() - normal * penetration);
    }
    else if (physicBody == PhysicBody::STATIC && otherBox->physicBody == PhysicBody::MOVABLE)
    {
        otherBox->SetPosition(otherBox->GetPosition() + normal * penetration);
    }
    else if (physicBody == PhysicBody::MOVABLE && otherBox->physicBody == PhysicBody::MOVABLE)
    {
        // split correction
        SetPosition(GetPosition() - normal * (penetration * 0.5f));
        otherBox->SetPosition(otherBox->GetPosition() + normal * (penetration * 0.5f));
    }


    // --- Phase 3 : collecte des points de contact ---
    auto halfA = GetHalfSize();
    auto halfB = otherBox->GetHalfSize();
    std::vector<PulseEngine::Vector3> contactPoints;

    for (int x = -1; x <= 1; x += 2)
    for (int y = -1; y <= 1; y += 2)
    for (int z = -1; z <= 1; z += 2)
    {
        PulseEngine::Vector3 corner = posA + PulseEngine::Vector3(x*halfA.x, y*halfA.y, z*halfA.z);
        float penetrationDepth = (posB - corner).Dot(normal);
        if (penetrationDepth >= 0)
            contactPoints.push_back(corner);
    }

    // fallback au centre si aucun coin trouvé
    if (contactPoints.empty())
        contactPoints.push_back(0.5f * (posA + posB));

    // --- Phase 4 : impulsion linéaire + rotation ---
    float e = 0.0f; // restitution zéro pour éviter rebonds

    auto InverseInertia = [](float m, const PulseEngine::Vector3& half) {
        PulseEngine::Vector3 I(
            (1.0f / 12.0f) * m * (half.y*half.y + half.z*half.z),
            (1.0f / 12.0f) * m * (half.x*half.x + half.z*half.z),
            (1.0f / 12.0f) * m * (half.x*half.x + half.y*half.y)
        );
        return PulseEngine::Vector3(1.0f / I.x, 1.0f / I.y, 1.0f / I.z);
    };

    PulseEngine::Vector3 invInertiaA = InverseInertia(mass, halfA);
    PulseEngine::Vector3 invInertiaB = InverseInertia(otherBox->mass, halfB);

    for (auto& contact : contactPoints)
    {
        PulseEngine::Vector3 rA = contact - posA;
        PulseEngine::Vector3 rB = contact - posB;

        PulseEngine::Vector3 vA = velocity + PulseEngine::Cross(angularVelocity, rA);
        PulseEngine::Vector3 vB = otherBox->velocity + PulseEngine::Cross(otherBox->angularVelocity, rB);

        PulseEngine::Vector3 relativeVel = vB - vA;
        float velAlongNormal = relativeVel.Dot(normal);
        if (velAlongNormal > 0.0f) continue;

        PulseEngine::Vector3 raCrossN = PulseEngine::Cross(rA, normal);
        PulseEngine::Vector3 rbCrossN = PulseEngine::Cross(rB, normal);

        float angularFactorA = raCrossN.Dot(invInertiaA * raCrossN);
        float angularFactorB = rbCrossN.Dot(invInertiaB * rbCrossN);

        float j = -(1.0f + e) * velAlongNormal;
        j /= invMassA + invMassB + angularFactorA + angularFactorB;

        PulseEngine::Vector3 impulse = j * normal;

        // --- Appliquer l’impulsion sur A si movable ---
        if (physicBody == PhysicBody::MOVABLE)
        {
            velocity -= impulse * invMassA;
            angularVelocity -= invInertiaA * PulseEngine::Cross(rA, impulse);
            angularVelocity.x = this->constraintRotation[0] ? 0.0f : angularVelocity.x;
            angularVelocity.y = this->constraintRotation[1] ? 0.0f : angularVelocity.y;
            angularVelocity.z = this->constraintRotation[2] ? 0.0f : angularVelocity.z;
        }

        // --- Appliquer l’impulsion sur B si movable ---
        if (otherBox->physicBody == PhysicBody::MOVABLE)
        {
            otherBox->velocity += impulse * invMassB;
            otherBox->angularVelocity += invInertiaB * PulseEngine::Cross(rB, impulse);
            otherBox->angularVelocity.x = otherBox->constraintRotation[0]? 0.0f : otherBox->angularVelocity.x;
            otherBox->angularVelocity.y = otherBox->constraintRotation[1]? 0.0f : otherBox->angularVelocity.y;
            otherBox->angularVelocity.z = otherBox->constraintRotation[2]? 0.0f : otherBox->angularVelocity.z;
        }
    }

    // --- Damping angulaire pour stabiliser ---
    if (physicBody == PhysicBody::MOVABLE) angularVelocity *= 0.95f;
    if (otherBox->physicBody == PhysicBody::MOVABLE) otherBox->angularVelocity *= 0.95f;
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
    float radX = (-rotation->x) * (M_PI / 180.0f);
    float radY = (-rotation->y) * (M_PI / 180.0f);
    float radZ = (-rotation->z) * (M_PI / 180.0f);

    PulseEngine::Mat4 rotationMatrix = PulseEngine::MathUtils::Matrix::Identity();
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateZ(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->z));
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateY(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->y));
    rotationMatrix = PulseEngine::MathUtils::Matrix::RotateX(rotationMatrix, PulseEngine::MathUtils::ToRadians(-rotation->x));
    

    switch (index)
    {
        case 0: return rotationMatrix * PulseEngine::Vector3(1, 0, 0); // Right / X
        case 1: return rotationMatrix * PulseEngine::Vector3(0, 1, 0); // Up    / Y
        case 2: return rotationMatrix * PulseEngine::Vector3(0, 0, 1); // Forward / Z
        default: return PulseEngine::Vector3(0, 0, 0);
    }
}


bool BoxCollider::SAT_MinimumTranslation(const BoxCollider& B, PulseEngine::Vector3& outAxis, float& outDepth) const
{
    if ((rotation->x - B.rotation->x) < 1e-3f &&
    (rotation->y - B.rotation->y) < 1e-3f &&
    (rotation->z - B.rotation->z) < 1e-3f)
    {
        return const_cast<BoxCollider*>(&B)->FastCheckCollision(const_cast<BoxCollider*>(this));
    }
    // axes locaux (unit)
    PulseEngine::Vector3 Aaxes[3] = { GetAxis(0), GetAxis(1), GetAxis(2) };
    PulseEngine::Vector3 Baxes[3] = { B.GetAxis(0), B.GetAxis(1), B.GetAxis(2) };

    // demi-extents locaux
    PulseEngine::Vector3 halfA = this->GetHalfSize();
    PulseEngine::Vector3 halfB = B.GetHalfSize();

    // rotation matrix R and AbsR: R[i][j] = dot(Ai, Bj)
    float R[3][3];
    float AbsR[3][3];
    const float EPS = 1e-6f;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[i][j] = PulseEngine::Dot(Aaxes[i], Baxes[j]);
            AbsR[i][j] = std::abs(R[i][j]) + EPS;
        }
    }

    // vector t from A to B in world coords, then expressed in A's frame
    PulseEngine::Vector3 centerA = this->GetCenter() + this->decalPosition;
    PulseEngine::Vector3 centerB = B.GetCenter() + B.decalPosition;
    PulseEngine::Vector3 tWorld = centerB - centerA;
    PulseEngine::Vector3 t( PulseEngine::Dot(tWorld, Aaxes[0]),
                            PulseEngine::Dot(tWorld, Aaxes[1]),
                            PulseEngine::Dot(tWorld, Aaxes[2]) );

    // track minimal penetration
    float minPen = FLT_MAX;
    PulseEngine::Vector3 minAxis(0,0,0);

    auto checkAxisA = [&](int i)->bool {
        float ra = halfA[i];
        float rb = halfB.x * AbsR[i][0] + halfB.y * AbsR[i][1] + halfB.z * AbsR[i][2];
        float dist = std::abs(t[i]);
        float overlap = ra + rb - dist;
        if (overlap < 0.0f) return false;
        if (overlap < minPen) { minPen = overlap; minAxis = Aaxes[i]; }
        return true;
    };

    auto checkAxisB = [&](int i)->bool {
        float ra = halfA.x * AbsR[0][i] + halfA.y * AbsR[1][i] + halfA.z * AbsR[2][i];
        float rb = halfB[i];
        float dist = std::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]);
        float overlap = ra + rb - dist;
        if (overlap < 0.0f) return false;
        if (overlap < minPen) { minPen = overlap; minAxis = Baxes[i]; }
        return true;
    };

    // test A axes
    for (int i = 0; i < 3; ++i) if (!checkAxisA(i)) return false;

    // test B axes
    for (int i = 0; i < 3; ++i) if (!checkAxisB(i)) return false;

    // test cross products A_i x B_j
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            // axis in world space
            PulseEngine::Vector3 axis = PulseEngine::Cross(Aaxes[i], Baxes[j]);
            float axisLen2 = axis.x*axis.x + axis.y*axis.y + axis.z*axis.z;
            float threshold = 1e-6f * (halfA.GetMagnitude() + halfB.GetMagnitude());
            if (axisLen2 < threshold*threshold) continue;

            // Express axis in A-frame for projection of t
            PulseEngine::Vector3 axisAframe = PulseEngine::Vector3(
                PulseEngine::Dot(axis, Aaxes[0]),
                PulseEngine::Dot(axis, Aaxes[1]),
                PulseEngine::Dot(axis, Aaxes[2])
            );

            // axis components on B axes
            PulseEngine::Vector3 axisBframe = PulseEngine::Vector3(
                PulseEngine::Dot(axis, Baxes[0]),
                PulseEngine::Dot(axis, Baxes[1]),
                PulseEngine::Dot(axis, Baxes[2])
            );

            PulseEngine::Vector3 axisN = axis / sqrt(axisLen2);
            float tProj = std::abs(PulseEngine::Dot(tWorld, axisN));

            float ra = halfA.x * std::abs(axisAframe.x) +
                       halfA.y * std::abs(axisAframe.y) +
                       halfA.z * std::abs(axisAframe.z);

            float rb = halfB.x * std::abs(axisBframe.x) +
                       halfB.y * std::abs(axisBframe.y) +
                       halfB.z * std::abs(axisBframe.z);

            float overlap = ra + rb - tProj;
            if (overlap < 0.0f) return false;
            if (overlap < minPen) { minPen = overlap; minAxis = axis; }
        }
    }

    // If we reach here => collision. minAxis points to axis of minimal penetration (world space)
    // normalize minAxis direction
    float len = std::sqrt(minAxis.x*minAxis.x + minAxis.y*minAxis.y + minAxis.z*minAxis.z);
    if (len < 1e-6f) return false;

    outAxis = minAxis * (1.0f / len);
    outDepth = minPen;
    return true;
}

