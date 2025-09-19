#ifndef PULSE_ENGINE_CORE_COROUTINE_H
#define PULSE_ENGINE_CORE_COROUTINE_H

class Coroutine
{
public:
    virtual ~Coroutine() = default;
    virtual bool Update(float deltaTime) = 0; // returns false = coroutine done
};

#endif
