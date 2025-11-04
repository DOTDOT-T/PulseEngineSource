#pragma once

#include "PulseEngine/core/Math/Vector.h"

/**
 * @brief Structure representing a single vertex in 3D space.
 * Includes position, normal, texture coordinates, bone IDs, and weights.
 */
struct Vertex
{
    PulseEngine::Vector3 Position;    ///< Vertex position in 3D space.
    PulseEngine::Vector3 Normal;      ///< Vertex normal vector.
    PulseEngine::Vector2 TexCoords;   ///< Texture coordinates (UV).
    PulseEngine::iVector4 BoneIDs;    ///< IDs of the bones affecting this vertex.
    PulseEngine::Vector4 Weights = PulseEngine::Vector4(0.0f);     ///< Weights corresponding to each bone.
    PulseEngine::Vector3 Tangent;
    PulseEngine::Vector3 Bitangent;
};