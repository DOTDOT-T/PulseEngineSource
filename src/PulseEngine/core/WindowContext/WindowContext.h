#ifndef WINDOWCONTEXT_H
#define WINDOWCONTEXT_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/PulseEngineBackend.h"

class PULSE_ENGINE_DLL_API WindowContext
{
public:
    bool Initialize(const char* title, int width, int height);
    void Shutdown();

    void PollEvents() const;
    void SwapBuffers() const;
    bool ShouldClose() const;

    GLFWwindow* GetGLFWWindow() { return window; }
    void SetGLFWWindow(GLFWwindow* newWindow) { window = newWindow; }

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* window = nullptr;
};


#endif