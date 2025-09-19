#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Common/common.h"
#include "Common/dllExport.h"

class Mesh;

class PULSE_ENGINE_DLL_API Primitive
{
    public:
    static Mesh* Cube();
    static Mesh* Sphere();
};

#endif