#include "FileReader.h"

FileReader::FileReader(const std::string &path)
{
#ifdef PULSE_WINDOWS
    fileData = new std::ifstream(path);
#endif
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
    std::ifstream* file = ReinterprateFileType<std::ifstream*>();
    nlohmann::json js;
    file << js;
    return js;
#endif

    return nlohmann::json();
}
