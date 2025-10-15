#ifndef DISKARCHIVE_H
#define DISKARCHIVE_H

#include "PulseEngine/core/FileManager/Archive/Archive.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"
#include "common/common.h"
#include "common/dllExport.h"

class PULSE_ENGINE_DLL_API DiskArchive : public Archive
{
public:
    DiskArchive(const std::string& path, Mode mode)
        : Archive(mode), fileReader(path)
    {
        if (IsLoading()) buffer = fileReader.ReadAll(); // read entire file into memory
        else buffer.clear();
        cursor = 0;
    }

    void Serialize(const char* name, int& value) override {
        if (IsSaving()) AppendToBuffer(reinterpret_cast<char*>(&value), sizeof(value));
        else ReadFromBuffer(reinterpret_cast<char*>(&value), sizeof(value));
    }

    void Serialize(const char* name, float& value) override 
    {
        if (IsSaving()) AppendToBuffer(reinterpret_cast<char*>(&value), sizeof(value));
        else ReadFromBuffer(reinterpret_cast<char*>(&value), sizeof(value));
    }

    void Serialize(const char* name, std::string& value) override 
    {
        if (IsSaving()) 
        {
            uint32_t len = static_cast<uint32_t>(value.size());
            Serialize("len", reinterpret_cast<int&>(len));
            AppendToBuffer(value.data(), len);
        } else 
        {
            uint32_t len = 0;
            Serialize("len", reinterpret_cast<int&>(len));
            if (cursor + len > buffer.size()) 
            {
                EDITOR_WARN("Archive: string data missing in buffer");
                return;
            }
            value.resize(len);
            ReadFromBuffer(value.data(), len);
        }
    }

    void Finalize() 
    {
        if (IsSaving()) 
        {
            fileReader.WriteAll(buffer); // save buffer to disk
        }
    }

private:
    FileReader fileReader;
    std::vector<char> buffer;
    size_t cursor;

    void AppendToBuffer(const char* data, size_t size) 
    {
        buffer.insert(buffer.end(), data, data + size);
    }

    void ReadFromBuffer(char* out, size_t size) 
    {
        if (cursor + size > buffer.size()) 
        {
            EDITOR_WARN("Archive: attempted to read past buffer end");
            return;
        }
        memcpy(out, buffer.data() + cursor, size);
        cursor += size;
    }
};


#endif