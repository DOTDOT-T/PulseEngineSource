#include "FileManager.h"
#include "PulseEngine/core/PulseEngineBackend.h"

#include <fstream>
#include <algorithm>

using json = nlohmann::json;

std::filesystem::path FileManager::workingDirectory = "";

static bool EndsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string FileManager::GetCollectionByExtension(const std::string& fileName)
{
    std::string collectionType = "guidCollection";

    if (EndsWith(fileName, ".mat"))
    {
        collectionType += "Materials";
    }
    else if (EndsWith(fileName, ".pmesh"))
    {
        collectionType += "Meshes";
    }
    else if (EndsWith(fileName, ".pEntity"))
    {
        collectionType += "Entities";
    }
    else if (EndsWith(fileName, ".pTexture"))
    {
        collectionType += "Textures";
    }

    collectionType += ".puid";
    return collectionType;
}

json FileManager::OpenEngineConfigFile(PulseEngineBackend* engine)
{
    const std::string configPath = std::string(ASSET_PATH) + "Engine-config.ini";
    json engineConfig;
    std::ifstream configFile(configPath);

    if (!configFile.is_open())
    {
        // Default engine config
        engineConfig["GameData"]["Name"]             = "Pulse game name";
        engineConfig["GameData"]["FirstScene"]        = "Scenes/CZEFGZEOUFHDZOEIDYO.pmap";
        engineConfig["GameData"]["DefaultResolution"] = "1920x1080";
        engineConfig["GameData"]["Fullscreen"]        = false;
        engineConfig["GameData"]["VSync"]             = true;

        engineConfig["Engine"]["Name"]     = engine->GetEngineName();
        engineConfig["Engine"]["Version"]  = engine->GetEngineVersion();
        engineConfig["Engine"]["DevMonth"] = engine->GetDevMonth();
        engineConfig["Engine"]["Author"]   = engine->GetCompanyName();
        engineConfig["Engine"]["License"]  = "None";
        engineConfig["Engine"]["discord"] = false;
        

        std::ofstream outFile(configPath);
        if (outFile.is_open())
        {
            outFile << engineConfig.dump(4);
            outFile.close();
        }
        return engineConfig;
    }

    configFile >> engineConfig;
    configFile.close();
    return engineConfig;
}

void FileManager::SaveEngineConfigFile(PulseEngineBackend* engine, const json& engineConfig)
{
    const std::string configPath = std::string(ASSET_PATH) + "Engine-config.ini";
    std::ofstream outFile(configPath);
    if (outFile.is_open())
    {
        outFile << engineConfig.dump(4);
        outFile.close();
    }
}

FileType FileManager::GetFileType(const std::string &fileName)
{
    EDITOR_LOG(fileName)
    if (EndsWith(fileName, ".png") || EndsWith(fileName, ".jpg") || EndsWith(fileName, ".jpeg"))
    {
        return FileType::TEXTURE;
    }
    else if (EndsWith(fileName, ".fbx") || EndsWith(fileName, ".obj"))
    {
        return FileType::MESH;
    }
    else if (EndsWith(fileName, ".pEntity"))
    {
        return FileType::PULSE_ENTITY;
    }
    else if (EndsWith(fileName, ".pmap"))
    {
        return FileType::MAP;
    }
    else if (EndsWith(fileName, ".cpp"))
    {
        return FileType::SCRIPT_CPP;
    }
    else if (EndsWith(fileName, ".h"))
    {
        return FileType::SCRIPT_H;
    }
    else if (EndsWith(fileName, ".synapse"))
    {
        return FileType::SYNAPSE;
    }
    

    return FileType::UNKNOWN;
}

std::vector<std::filesystem::path> FileManager::GetFilesInDirectoryWithExtension(const std::string &directory, const std::string &extension)
{
    std::vector<std::filesystem::path> files;
    std::filesystem::path dirPath(directory);

    if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
    {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
            {
                files.push_back(entry.path());
            }
        }
    }
    return files;
}

bool FileManager::CreateNewDirectories(const std::string &path)
{
    std::vector<std::string> dirs;
    std::string temp;

    std::string finalPath = std::string(ASSET_PATH) + path;

    // get all the different directories from the path
    for (char ch : finalPath)
    {
        if (ch == '/' || ch == '\\')
        {
            if (!temp.empty())
            {
                dirs.push_back(temp);
                temp.clear();
            }
        }
        else
        {
            temp += ch;
        }
    }

    if (!temp.empty())
    {
        dirs.push_back(temp);
    }

    std::filesystem::path currentPath;

    //generate directories by concatenating the last directory to the next one
    for (const auto& dir : dirs)
    {
        currentPath /= dir;
        if (!std::filesystem::exists(currentPath))
        {
            if (!std::filesystem::create_directory(currentPath))
            {
                return false; // Failed to create directory
            }
        }
    }

    return true;
}
