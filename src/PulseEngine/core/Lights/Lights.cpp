#include "Lights.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"

void LightData::Serialize(Archive &ar)
{
    Entity::Serialize(ar);
    ar.Serialize("lightColor.r", color.r);
    ar.Serialize("lightColor.g", color.g);
    ar.Serialize("lightColor.b", color.b);
    ar.Serialize("lightColor.a", color.a);
}


void LightData::Deserialize(Archive& ar)
{

}

const char* LightData::ToString()
{
    return "LightData";
}