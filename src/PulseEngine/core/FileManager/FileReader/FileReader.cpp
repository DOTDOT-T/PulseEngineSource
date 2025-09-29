#include "FileReader.h"

FileReader::FileReader(const std::string &path)
{
    std::string definePath = std::string(ASSET_PATH) + path;

#ifdef PULSE_WINDOWS
    std::filesystem::path p(definePath);
    if(!std::filesystem::exists(p))
    {
        EDITOR_LOG("File at path " + definePath + " does not exist. Creating it");
        FileManager::CreateNewDirectories(path.substr(0, path.find_last_of("/\\")));
        std::ofstream createFile(definePath);
        createFile.close();
    }
    fileData = new std::ifstream(definePath);
#endif

    EDITOR_LOG("File at path " + definePath + " opened.");
}

FileReader::~FileReader()
{
#ifdef PULSE_WINDOWS
    delete static_cast<std::ifstream*>(fileData);
#endif
}

nlohmann::json FileReader::ToJson()
{
#ifdef PULSE_WINDOWS
    std::ifstream* file = ReinterprateFileType<std::ifstream>();
    nlohmann::json js;
    try {
        (*file) >> js;
    } catch (const std::exception& e) {
        EDITOR_LOG(std::string("JSON parse error: ") + e.what());
    }
    EDITOR_LOG(js.dump(4))
    return js;
#endif

    return nlohmann::json();
}

void FileReader::SaveJson(const nlohmann::json &js)
{
#ifdef PULSE_WINDOWS
    std::ofstream* file = ReinterprateFileType<std::ofstream>();
    if(file->is_open())
    {
        (*file) << js.dump(4);
        file->flush();
    }
#endif
}

bool FileReader::IsOpen()
{
#ifdef PULSE_WINDOWS
    std::ifstream* file = ReinterprateFileType<std::ifstream>();
    return file->is_open();
#endif

    return false;
}

void FileReader::Close()
{
#ifdef PULSE_WINDOWS
    std::ifstream* file = ReinterprateFileType<std::ifstream>();
    if(file->is_open())
        file->close();
#endif
}
