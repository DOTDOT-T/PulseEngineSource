#ifndef MATERIALAPI_H
#define MATERIALAPI_H

#include "Common/common.h"
#include "Common/dllExport.h"

class Material;
class Shader;

class PULSE_ENGINE_DLL_API MaterialApi
{
    private:
        Material* material;
    public:
        MaterialApi(Material* m);

        Shader* GetShader();
        void SetShader(Shader* shader);

        void ChangeColor(float r, float g, float b);
};

#endif