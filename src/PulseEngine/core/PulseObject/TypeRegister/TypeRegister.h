#ifndef TYPEREGISTER_H
#define TYPEREGISTER_H

#include "Common/dllExport.h"

#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <memory>
#include "PulseEngine/core/PulseObject/PulseObject.h"

#define PULSE_REGISTER_CLASS_HEADER(name) \
    public: \
        static bool objectRegistered##name;

#define PULSE_REGISTER_CLASS_CPP(name) \
        bool name::objectRegistered##name = [](){ \
            TypeRegistry::RegisterType(#name, []() -> PulseObject* { return new name(); }); \
            return true; \
        }(); 

using ObjectFactory = std::function<PulseObject*()>;

class PULSE_ENGINE_DLL_API TypeRegistry
{
public:
    static void RegisterType(const std::string& typeName, ObjectFactory factory)
    {
        std::lock_guard<std::mutex> lock(GetMutex());
        auto& map = GetMap();
        map[typeName] = factory;
    }

    static PulseObject* CreateInstance(const std::string& typeName)
    {
        std::lock_guard<std::mutex> lock(GetMutex());
        auto& map = GetMap();
        auto it = map.find(typeName);
        if (it != map.end())
            return it->second();
        return nullptr; 
    }

    static bool IsRegistered(const std::string& typeName)
    {
        std::lock_guard<std::mutex> lock(GetMutex());
        auto& map = GetMap();
        return map.find(typeName) != map.end();
    }

    static std::unordered_map<std::string, ObjectFactory>& GetMap()
    {
        static std::unordered_map<std::string, ObjectFactory> map;
        return map;
    }

private:
    static std::mutex& GetMutex()
    {
        static std::mutex mtx;
        return mtx;
    }
};




#endif