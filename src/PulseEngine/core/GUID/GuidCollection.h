/**
 * @file GuidCollection.h
 * @author ...
 * @brief Provides asset identification and lookup through GUIDs.
 * 
 * @details
 * The GuidCollection class manages a set of file paths associated with
 * globally unique identifiers (GUIDs). It serves as the engine’s internal
 * asset mapping layer, allowing systems to reference resources by GUID
 * instead of absolute or relative file paths.
 * 
 * Each collection corresponds to a specific asset domain
 * (e.g., textures, materials, meshes) and can be serialized/deserialized
 * to ensure persistent asset references across sessions.
 * 
 * @section Responsibilities
 * - Maintain a bi-directional mapping between asset GUIDs and file paths.
 * - Provide fast lookup for assets during load and runtime.
 * - Support consistent asset referencing across editor and engine runtime.
 * 
 * @section Example
 * @code
 *  GuidCollection textureCollection("assets/textures.collection");
 *  textureCollection.InsertFile("assets/textures/wood_diffuse.png");
 *  std::string path = textureCollection.GetFilePathFromGuid("8a12b...f1");
 * @endcode
 * 
 * @version 0.1
 * @date 2025-10-14
 * 
 * @see AssetManager
 * @see GuidGenerator
 * @note GUIDs are stored as strings and must be unique per collection.
 * @warning Not thread-safe. External synchronization is required if used concurrently.
 */

#ifndef GUIDCOLLECTION_H
#define GUIDCOLLECTION_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include <unordered_map>
#include <string>

/**
 * @class GuidCollection
 * @brief Stores and manages the mapping between asset GUIDs and file paths.
 * 
 * @details
 * This class encapsulates the concept of an "asset collection" identified by name.
 * Internally, it maintains a hash map that associates each asset’s unique GUID
 * with its absolute or relative file path on disk.
 * 
 * It acts as a lightweight registry that can be loaded by the engine or editor
 * to resolve asset references at runtime.
 */
class PULSE_ENGINE_DLL_API GuidCollection
{
private:
    /// @brief Name of the collection (usually derived from its file or category).
    std::string collectionName;

    /// @brief Map linking GUID strings to their corresponding file paths.
    /// @note Keys are GUIDs; values are relative or absolute paths.
    std::unordered_map<std::string, std::string> files;

public:
    /**
     * @brief Constructs a GuidCollection and optionally loads it from disk.
     * 
     * @param collectionPath Path to the collection file or resource folder.
     */
    GuidCollection(const std::string& collectionPath);

    /// @brief Destructor.
    ~GuidCollection() {}

    /// @brief Returns the name of this GUID collection.
    std::string GetCollectionName() const { return collectionName; }

    /// @brief Returns all GUID-to-file mappings.
    std::unordered_map<std::string, std::string> GetFiles() const { return files; }

    /**
     * @brief Inserts a file into the collection and assigns it a GUID.
     * 
     * @param filePath The path to the file being registered.
     * @return The total number of files in the collection after insertion.
     * 
     * @note If the file already exists in the collection, the existing GUID is reused.
     */
    std::size_t InsertFile(const std::string& filePath);

    /**
     * @brief Retrieves the file path corresponding to a given GUID.
     * 
     * @param guid The unique identifier of the asset.
     * @return The file path associated with that GUID, or an empty string if not found.
     */
    std::string GetFilePathFromGuid(const std::string& guid) const;

    std::string GetGuidFromFilePath(const std::string& filePath) const;
};

#endif // GUIDCOLLECTION_H
