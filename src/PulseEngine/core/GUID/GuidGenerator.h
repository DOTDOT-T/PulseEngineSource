/**
 * @file GuidGenerator.h
 * @author Dorian LEXTERIAQUE
 * @brief Provides GUID generation utilities for the Pulse Engine asset system.
 * 
 * @details
 * This header defines functions that generate globally unique identifiers (GUIDs)
 * for files and assets. GUIDs are used throughout the engine to uniquely reference
 * resources, decoupling them from file paths and enabling safe renaming or relocation
 * of assets.
 * 
 * The functions here are used by the GuidCollection system, AssetManager, and
 * other serialization or resource-tracking systems.
 * 
 * @section Usage Example
 * @code
 *  std::size_t texGUID = GenerateGUIDFromPath("assets/textures/wood.png");
 *  std::size_t meshGUID = GenerateGUIDFromPathAndMap("assets/meshes/cube.obj", "default");
 * @endcode
 * 
 * @note GUIDs are deterministic based on input path (and optional map name), ensuring
 * consistent asset identification across sessions and editor reloads.
 * 
 * @version 0.2
 * @date 2025-10-14
 * 
 * @see GuidCollection
 * @copyright Copyright (c) 2025
 */

#ifndef GUIDGENERATOR_H
#define GUIDGENERATOR_H

#include "Common/common.h"
#include "Common/dllExport.h"

/**
 * @brief Generates a deterministic GUID from a file path string.
 * 
 * @param filepath Path to the asset file.
 * @return A size_t representing the GUID.
 * 
 * @note Deterministic: calling this function multiple times with the same
 * filepath will return the same GUID.
 */
std::size_t PULSE_ENGINE_DLL_API GenerateGUIDFromPath(const std::string& filepath);

/**
 * @brief Generates a deterministic GUID from a file path and a map name.
 * 
 * @param filepath Path to the asset file.
 * @param mapName Optional map or collection name to namespace the GUID.
 * @return A size_t representing the GUID.
 * 
 * @note Using a map name allows multiple collections to reference the same
 * file independently without collision.
 */
std::size_t PULSE_ENGINE_DLL_API GenerateGUIDFromPathAndMap(const std::string& filepath, const std::string& mapName);

#endif // GUIDGENERATOR_H
