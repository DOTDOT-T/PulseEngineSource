#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include "Common/dllExport.h"

#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Mat3.h"
#include "PulseEngine/core/Math/Vector.h"

class IGraphicsAPI;


class PULSE_ENGINE_DLL_API Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath, IGraphicsAPI* graphicApi);
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, IGraphicsAPI* graphicApi);
    unsigned int getProgramID() const { return shaderID; }
    void Use() const;
    void SetMat4(const std::string& name, const PulseEngine::Mat4& mat) const;
    void SetMat3(const std::string& name, const PulseEngine::Mat3& mat) const;
    void SetVec3(const std::string& name, const PulseEngine::Vector3& vec) const;
    void SetFloat(const std::string& name, float fl) const;
    void SetBool(const std::string& name, bool b) const;
    void SetInt(const std::string& name, int value) const;
    void SetIntArray(const std::string& name, const int* values, int count) const;


    IGraphicsAPI* graphics = nullptr;
private:
    unsigned int shaderID;

};

#endif // SHADER_H
