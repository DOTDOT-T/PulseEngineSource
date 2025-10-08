#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp> // Assimp::Importer

#include "Common/common.h"
#include "Common/dllExport.h"

class Skeleton;
class Shader;
class SkeletalMesh;
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
    PulseEngine::Vector4 Weights;     ///< Weights corresponding to each bone.
    PulseEngine::Vector3 Tangent;
    PulseEngine::Vector3 Bitangent;
};

/**
 * @brief Represents a 3D mesh including geometry, bones, and rendering data.
 * This class handles OpenGL buffer setup and rendering, and can import mesh data via Assimp.
 */
class PULSE_ENGINE_DLL_API Mesh
{
public:
    /**
     * @brief Constructor with raw vertex data (float format).
     * @param vertices Array of vertex data as floats.
     */
    Mesh(const std::vector<float>& vertices);

    /**
     * @brief Constructor with raw vertex data and index data.
     * @param vertices Array of vertex data as floats.
     * @param indices Array of index data.
     */
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Default constructor.
     */
    Mesh();

    /**
     * @brief Destructor. Cleans up GPU buffers.
     */
    ~Mesh();

    /**
     * @brief Draws the mesh using the specified shader program.
     * @param shaderProgram OpenGL shader program ID.
     */
    void Draw(Shader* shader);

    /**
     * @brief Loads mesh data from an Assimp mesh object.
     * @param mesh Assimp mesh pointer.
     * @param scene Assimp scene containing the mesh.
     * @return Pointer to a new Mesh object.
     */
    static Mesh* LoadFromAssimp(const aiMesh* mesh, const aiScene* scene, SkeletalMesh* skel = nullptr);

    /**
     * @brief Updates the mesh state (for animation or other time-based effects).
     * @param deltaTime Time elapsed since the last update.
     */
    void UpdateMesh(float deltaTime);

    /**
     * @brief Assigns bone influence data to a vertex.
     * @param vertex Vertex to modify.
     * @param boneID ID of the bone.
     * @param weight Influence weight of the bone.
     */
    static void AddBoneDataToVertex(Vertex& vertex, int boneID, float weight);

    /// Pointer to the skeleton associated with this mesh (for skeletal animation).
    Skeleton* skeleton = nullptr;

    /// Assimp importer used for loading mesh files.
    Assimp::Importer* importer;

    // disallow copies
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // allow move
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }

    std::size_t GetGuid() const { return guid; }
    void SetGuid(std::size_t newGuid) { guid = newGuid; }

    PulseEngine::Vector3 position = PulseEngine::Vector3(0.0f, 0.0f, 0.0f); ///< Position of the mesh in local space.
    PulseEngine::Vector3 rotation = PulseEngine::Vector3(0.0f, 0.0f, 0.0f); ///< Rotation of the mesh in local space.
    PulseEngine::Vector3 scale = PulseEngine::Vector3(1.0f, 1.0f, 1.0f); ///< Scale of the mesh in local space.

    PulseEngine::Mat4 matrix;
private:
    /**
     * @brief Initializes the OpenGL buffers (VAO, VBO, EBO) for rendering.
     */
    void SetupMesh();

    std::vector<Vertex> vertices;         ///< Final vertex data including bone weights.
    std::vector<PulseEngine::Vector3> normals;       ///< Normal vectors (used before conversion).
    std::vector<PulseEngine::Vector2> texCoords;     ///< Texture coordinates (used before conversion).
    std::vector<unsigned int> indices;    ///< Index data for rendering (EBO).

    std::string name;
    
    std::size_t guid = 0;
    std::size_t muid = 0;

    unsigned int VAO; ///< Vertex Array Object.
    unsigned int VBO; ///< Vertex Buffer Object.
    unsigned int EBO; ///< Element Buffer Object.
};
