#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "common/common.h"
#include "common/dllExport.h"
#include "PulseEngine/core/PulseObject/PulseObject.h"


class Entity;


class PULSE_ENGINE_DLL_API SceneManager : public PulseObject
{
public:
    SceneManager* GetInstance();

private:
    SceneManager();
    SceneManager(const SceneManager& sm) = delete;
    ~SceneManager() = delete;

    std::unordered_map<uint64_t, Entity*> entities;
};

#endif