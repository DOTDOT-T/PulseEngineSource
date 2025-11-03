#ifndef DISKARCHIVE_H
#define DISKARCHIVE_H

#include "PulseEngine/core/FileManager/Archive/Archive.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"
#include "common/common.h"
#include "common/dllExport.h"

#include <vector>
#include <string>
#include <cstring>
#include <filesystem>
using namespace PulseEngine::FileSystem;

// ============================================================================
// DiskArchive : Archive binaire fiable pour lecture/écriture disque
// - Gestion stricte du curseur
// - Vérification d’intégrité
// - Sérialisation type-safe
// - Logging clair
// ============================================================================
class PULSE_ENGINE_DLL_API DiskArchive : public Archive
{
public:
    explicit DiskArchive(const std::string& path, Mode mode)
        : Archive(mode), fileReader(path), cursor(0)
    {
        if (IsLoading())
        {
            buffer = fileReader.ReadAll();
            if (buffer.empty())
            {
                EDITOR_WARN("DiskArchive: unable to read file or empty buffer (" << path << ")");
            }
        }
        else
        {
            buffer.clear();
        }

        // (Optionnel) Magic header/version
        if (IsSaving())
        {
            const uint32_t magic = 0x504C5345; // "PLSE"
            AppendToBuffer(reinterpret_cast<const char*>(&magic), sizeof(magic));
        }
        else
        {
            uint32_t magic = 0;
            ReadFromBuffer(reinterpret_cast<char*>(&magic), sizeof(magic));
            if (magic != 0x504C5345)
            {
                EDITOR_WARN("DiskArchive: invalid or incompatible file format (" << path << ")");
                cursor = 0; // tente quand même de continuer
            }
        }
    }

    // =========================================================================
    // Sérialisation type-safe
    // =========================================================================
    void Serialize(const char* name, int& value) override {
        SerializePrimitive(value);
        EDITOR_LOG("int -> " << name << " : " << value)
    }

    void Serialize(const char* name, float& value) override {
        SerializePrimitive(value);
        EDITOR_LOG("float -> " << name << " : " << value)
    }
    void Serialize(const char* name, std::uint64_t& value) override {
        SerializePrimitive(value);
        EDITOR_LOG("uint64_t -> " << name << " : " << value)
    }

    void Serialize(const char* name, uint32_t& value) override {
        SerializePrimitive(value);
        EDITOR_LOG("int -> " << name << " : " << value)
    }

    void Serialize(const char* name, std::string& value) override
    {
        EDITOR_LOG("string -> " << name << " : " << value)
        if (IsSaving())
        {
            uint32_t len = static_cast<uint32_t>(value.size());
            Serialize("len", len);
            AppendToBuffer(value.data(), len);
        }
        else
        {
            uint32_t len = 0;
            Serialize("len", len);

            if (cursor + len > buffer.size())
            {
                EDITOR_WARN("DiskArchive: string data missing (" << len << " bytes requested, "
                    << (buffer.size() - cursor) << " available)");
                value.clear();
                return;
            }

            value.resize(len);
            ReadFromBuffer(value.data(), len);
            EDITOR_LOG("DiskArchive: read string [" << value << "] (" << len << " bytes)");
        }
    }

    // =========================================================================
    // Finalisation
    // =========================================================================
    void Finalize()
    {
        if (IsSaving())
        {
            fileReader.WriteAll(buffer);
            EDITOR_LOG("DiskArchive: wrote " << buffer.size() << " bytes to disk.");
        }
    }

private:
    FileReader fileReader;
    std::vector<char> buffer;
    size_t cursor;

    // =========================================================================
    // Fonctions internes
    // =========================================================================
    template<typename T>
    void SerializePrimitive(T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "SerializePrimitive requires trivially copyable type");

        if (IsSaving())
        {
            AppendToBuffer(reinterpret_cast<const char*>(&value), sizeof(T));
        }
        else
        {
            ReadFromBuffer(reinterpret_cast<char*>(&value), sizeof(T));
        }
    }

    void AppendToBuffer(const char* data, size_t size)
    {
        if (!data || size == 0) return;
        buffer.insert(buffer.end(), data, data + size);
    }

    void ReadFromBuffer(char* out, size_t size)
    {
        if (cursor + size > buffer.size())
        {
            EDITOR_WARN("DiskArchive: attempted to read past end (" << cursor << " + " << size
                << " > " << buffer.size() << ")");
            cursor = buffer.size();
            memcpy(out, (char*)"0", strlen("0"));


            return;
        }

        memcpy(out, buffer.data() + cursor, size);
        cursor += size;
    }
};

#endif // DISKARCHIVE_H
