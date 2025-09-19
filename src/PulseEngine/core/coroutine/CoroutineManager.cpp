#include "CoroutineManager.h"
#include "PulseEngine/core/coroutine/Coroutine.h"

void CoroutineManager::Add(std::unique_ptr<Coroutine> coroutine)
{
    coroutines.emplace_back(std::move(coroutine));
}

void CoroutineManager::UpdateAll(float deltaTime)
{
    for (auto it = coroutines.begin(); it != coroutines.end(); )
    {
        if (!(*it)->Update(deltaTime))
        {
            it = coroutines.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
