#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "common/common.h"
#include "common/dllExport.h"

class PULSE_ENGINE_DLL_API Archive
{
public:
    enum class Mode { Saving, Loading };

    Archive(Mode m) : mode(m) {}
    virtual ~Archive() = default;

    Mode GetMode() const { return mode; }
    bool IsSaving() const { return mode == Mode::Saving; }
    bool IsLoading() const { return mode == Mode::Loading; }

    // Core API for primitive types
    virtual void Serialize(const char* name, int& value) = 0;
    virtual void Serialize(const char* name, std::size_t& value) = 0;
    virtual void Serialize(const char* name, float& value) = 0;
    virtual void Serialize(const char* name, uint32_t& value) = 0;
    virtual void Serialize(const char* name, std::string& value) = 0;

protected:
    Mode mode;
};


#endif