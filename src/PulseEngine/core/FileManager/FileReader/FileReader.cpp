#include "FileReader.h"
using namespace PulseEngine::FileSystem;

FileReader::FileReader(const std::string &path)
{
    filePath = path;
    std::string definePath = std::string(ASSET_PATH) + path;

#ifdef PULSE_WINDOWS
    std::filesystem::path p(definePath);
    if (!std::filesystem::exists(p))
    {
        EDITOR_LOG("File at path " + definePath + " does not exist. Creating it");
    
        auto parentDir = std::filesystem::path(definePath).parent_path();
        std::filesystem::create_directories(parentDir);
    
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
    std::ifstream* file = ReinterprateFileType<std::ifstream>();
    file->close();
    std::ofstream outFile(std::string(ASSET_PATH) + filePath);
    if(outFile.is_open())
    {
        outFile << js.dump(4);
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

std::vector<char> FileReader::ReadAll()
{
#ifdef PULSE_WINDOWS
    std::ifstream* file = new std::ifstream(std::string(ASSET_PATH) + filePath, std::ios::binary);
    
    // Ensure file is open in binary mode
    if (!file->is_open())
        return {};

    // Get file size
    file->seekg(0, std::ios::end);
    std::streamsize size = file->tellg();
    file->seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (size > 0)
        file->read(buffer.data(), size);

    return buffer;
#endif
}

void FileReader::WriteAll(const std::vector<char>& buffer)
{
#ifdef PULSE_WINDOWS
    std::ofstream* file = new std::ofstream(std::string(ASSET_PATH) + filePath, std::ios::binary);

    // Ensure the file is open and ready
    if (!file || !file->is_open())
        return;

    // Write the entire buffer
    if (!buffer.empty())
        file->write(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    // Flush to make sure everything is written to disk
    file->flush();
#endif
}

