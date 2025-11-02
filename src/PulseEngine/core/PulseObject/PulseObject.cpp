#include "PulseObject.h"
// #include "Common/common.h"
#include "Common/EditorDefines.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

PulseObject::PulseObject(const char* name) : objectName(name), guid(GenerateGUIDFromPath(name))
{
}

PulseObject::~PulseObject()
{
}

void PulseObject::Serialize(Archive &ar)
{
    EDITOR_WARN("Serialization is not implemented for " << objectName)
}

void PulseObject::Deserialize(Archive &ar)
{
    EDITOR_WARN("Deserialization is not implemented for " << objectName)
}

const char *PulseObject::ToString()
{
    return "stringification of Pulse Object";
}

const char *PulseObject::GetTypeName() const
{
    return "PulseObject";
}
