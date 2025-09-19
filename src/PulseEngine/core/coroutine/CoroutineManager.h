#ifndef PULSE_ENGINE_CORE_COROUTINE_MANAGER_H
#define PULSE_ENGINE_CORE_COROUTINE_MANAGER_H

#include <vector>
#include <memory>

class Coroutine;

class CoroutineManager
{
private:
    std::vector<std::unique_ptr<Coroutine>> coroutines;

public:
    void Add(std::unique_ptr<Coroutine> coroutine);

    void UpdateAll(float deltaTime);
};

#endif