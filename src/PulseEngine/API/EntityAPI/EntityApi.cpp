#include "EntityApi.h"
#include "PulseEngine/API/MaterialAPI/MaterialApi.h"

PulseEngine::EntityApi::EntityApi(Entity *e) : entity(e), materialApi(new MaterialApi(e->GetMaterial()))
{
}

MaterialApi *PulseEngine::EntityApi::GetMaterialApi()
{
    return materialApi;
}
