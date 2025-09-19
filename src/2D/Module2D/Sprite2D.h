#ifndef SPRITE2D_H
#define SPRITE2D_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "PulseEngine/CustomScripts/IScripts.h"

class Texture;

// class PULSE_ENGINE_DLL_API Sprite2D
// {
//     public:
//     Sprite2D(const std::string& texturePath);
//     ~Sprite2D() {
//         glDeleteVertexArrays(1, &VAO);
//         glDeleteBuffers(1, &VBO);
//         glDeleteBuffers(1, &EBO);
//     }

//     void Update();
//     void Draw();

//     private:    
//     unsigned int VAO; ///< Vertex Array Object.
//     unsigned int VBO; ///< Vertex Buffer Object.
//     unsigned int EBO; ///< Element Buffer Object.

//     PulseEngine::Vector2 position;
//     PulseEngine::Vector2 scale;
//     PulseEngine::Color color;
//     float rotation;


//     Texture* texture; ///< Texture of the sprite.

// };

#endif