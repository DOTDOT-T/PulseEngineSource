#include "GuidCollection.h"
#include "GuidGenerator.h"

GuidCollection::GuidCollection(const std::string &collectionPath)
{
    std::ifstream file(std::string(ASSET_PATH) + "EngineConfig/" + collectionPath);
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
        if(value.is_string())
        {
            files[key] = value;
            EDITOR_LOG("Loaded GUID: " + key + " -> " + value.get<std::string>());
        }
        else
        {
            files[key] = value.dump();
            EDITOR_LOG("Loaded GUID: " + key + " -> " + value.dump());
        }
    }
}

std::size_t GuidCollection::InsertFile(const std::string &filePath)
{
    auto sanitizer = [&](const std::string& input)
    {
            std::string out;
    out.reserve(input.size());

    bool lastWasSlash = false;

    for (char c : input)
    {
        char normalized = (c == '\\') ? '/' : c;

        if (normalized == '/')
        {
            if (lastWasSlash)
                continue;           // skip double slash

            lastWasSlash = true;
        }
        else
        {
            lastWasSlash = false;
        }

        out.push_back(normalized);
    }

    return out;
    };

    std::string sanit = sanitizer(filePath);
    std::size_t guid = GenerateGUIDFromPath(sanit);
    std::string guidStr = std::to_string(guid);

    while (files.find(guidStr) != files.end())
    {
        //if the filepath is already inside, we didnt need to try to insert but return the guid
        if(files[guidStr] == sanit) return guid;
        guid++;
        guidStr = std::to_string(guid);
    }
    files[guidStr] = sanit;

    std::ifstream file(std::string(ASSET_PATH) + "EngineConfig/" + collectionName);
    if (!file.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file for reading: " + collectionName);
        return guid;
    }
    nlohmann::json_abi_v3_12_0::json jsonData;
    file >> jsonData;
    file.close();

    jsonData[guidStr] = filePath;

    std::ofstream outFile(std::string(ASSET_PATH) + "EngineConfig/" + collectionName);
    if (!outFile.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file for writing: " + collectionName);
        return guid;
    }
    outFile << jsonData.dump(4);
    outFile.close();
    EDITOR_LOG("Inserted new GUID: " + guidStr + " -> " + sanit);

    return guid;
}

std::string GuidCollection::GetGuidFromFilePath(const std::string &filePath) const
{
    for (const auto& [key, value] : files)
    {
        if (value == filePath)
        {
            return key;
        }
    }
    return std::string("");
}

bool GuidCollection::RemoveGuidFromCollection(const std::string &guid)
{
    nlohmann::json_abi_v3_12_0::json jsonData;
    std::ifstream file(std::string(ASSET_PATH) + "EngineConfig/" + collectionName);
    files.erase(guid);
    file >> jsonData;
    file.close();

    jsonData.erase(guid);

    std::ofstream outFile(std::string(ASSET_PATH) + "EngineConfig/" + collectionName);
    if (!outFile.is_open())
    {
        EDITOR_ERROR("Could not open GUID collection file for writing: " + collectionName);
        return false;
    }
    outFile << jsonData.dump(4);
    outFile.close();
    EDITOR_SUCCESS("Erased " << guid << " from " << collectionName);
    return true;
}
