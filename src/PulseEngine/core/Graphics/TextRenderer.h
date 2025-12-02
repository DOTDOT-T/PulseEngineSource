#pragma once
#include <string>
#include <vector>
#include "common/dllExport.h"


// ===============================
// ABSTRACT INTERFACE
// ===============================
class PULSE_ENGINE_DLL_API ITextRenderer
{
public:
    virtual ~ITextRenderer() = default;

    virtual bool Init() = 0;
    virtual void SetScreenSize(int w, int h) = 0;

    virtual void RenderText(const std::string& text,
                          float x, float y,
                          float size,
                          const PulseEngine::Vector3& color) = 0;

    virtual void Render() = 0;
};