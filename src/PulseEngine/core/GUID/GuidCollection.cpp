#include "GuidCollection.h"
#include "GuidGenerator.h"

GuidCollection::GuidCollection(const std::string &collectionPath)
{
    std::ifstream file(std::string(ASSET_PATH) + collectionPath);
    if (!file.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file: " + collectionPath);
        return;
    }

    nlohmann::json_abi_v3_12_0::json jsonData;
    file >> jsonData;
    file.close();

    collectionName = collectionPath;

    EDITOR_LOG("Loading GUID collection: " + collectionPath);
    for (auto& [key, value] : jsonData.items())
    {
        files[key] = value;
        EDITOR_LOG("Loaded GUID: " + key + " -> " + value.get<std::string>());
    }
}

std::size_t GuidCollection::InsertFile(const std::string &filePath)
{
    std::size_t guid = GenerateGUIDFromPath(filePath);

    std::string guidStr = std::to_string(guid);
    while (files.find(guidStr) != files.end())
    {
        guid++;
        guidStr = std::to_string(guid);
    }
    files[guidStr] = filePath;

    std::ifstream file(std::string(ASSET_PATH) + collectionName);
    if (!file.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file for reading: " + collectionName);
        return guid;
    }
    nlohmann::json_abi_v3_12_0::json jsonData;
    file >> jsonData;
    file.close();


    jsonData[guidStr] = filePath;

    std::ofstream outFile(std::string(ASSET_PATH) + collectionName);
    if (!outFile.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file for writing: " + collectionName);
        return guid;
    }
    outFile << jsonData.dump(4);
    outFile.close();
    EDITOR_LOG("Inserted new GUID: " + guidStr + " -> " + filePath);

    return guid;
}
