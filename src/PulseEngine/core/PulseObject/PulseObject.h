#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include <cstddef>
#include <stdio.h>
#include "Common/dllExport.h"

/**
 * @brief When generating a new class, the generation need to use PULSE_GEN_BODY(name of the class)
 * @details this permit the engine to register correctly the class and generate all the body needed for this.
 * 
 */
#define PULSE_GEN_BODY(name) \
    public: \
    const char* GetTypeName() const override { return #name; }

class Archive;

class PULSE_ENGINE_DLL_API PulseObject
{
    public:

        PulseObject(const char* name = "Unnamed");
        ~PulseObject();

        virtual void Serialize(Archive& ar);
        virtual void Deserialize(Archive& ar);
        virtual const char* GetTypeName() const;
    private:
        const char* objectName;
        std::size_t guid;
};

#endif