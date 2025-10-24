#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include <cstddef>
#include <cstdint>
#include <stdio.h>
#include "Common/dllExport.h"





/**
 * @brief To implement the class into the Pulse Ecosystem, use PULSE_GEN_BODY(ClassName)
 * @details this permit the engine to register correctly the class and generate all the body needed for this.
 * @note DECLARE IN CPP THESE FUNCTION :
 *  @note void Serialize(Archive& ar) override;
    @note void Deserialize(Archive& ar) override;
    @note const char* ToString() override;
 */
#define PULSE_GEN_BODY(name) \
    public: \
    const char* GetTypeName() const override { return #name; } \
    void Serialize(Archive& ar) override; \
    void Deserialize(Archive& ar) override; \
    const char* ToString() override; \

class Archive;

class PULSE_ENGINE_DLL_API PulseObject
{
    public:

        PulseObject(const char* name = "Unnamed");
        ~PulseObject();

        virtual void Serialize(Archive& ar);
        virtual void Deserialize(Archive& ar);
        virtual const char* ToString();
        virtual const char* GetTypeName() const;

        uint64_t GetGuid() {return static_cast<uint64_t>(guid);}
    private:
        const char* objectName;
        std::size_t guid;
};

#endif