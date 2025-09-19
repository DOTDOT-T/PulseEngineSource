#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in ivec4 aBoneIDs;
layout (location = 3) in vec4 aWeights;
layout(location = 4) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool hasSkeleton;
uniform mat4 boneTransforms[100];

void main()
{
    mat4 skinMatrix = mat4(1.0);
    TexCoords = aTexCoords;

    if (hasSkeleton)
    {
        // Appliquer l'influence de chaque os
        skinMatrix =
            boneTransforms[aBoneIDs[0]] * aWeights[0] +
            boneTransforms[aBoneIDs[1]] * aWeights[1] +
            boneTransforms[aBoneIDs[2]] * aWeights[2] +
            boneTransforms[aBoneIDs[3]] * aWeights[3];

        // Appliquer la transformation du modèle
        vec4 skinnedPos = model * skinMatrix * vec4(aPos, 1.0);
        FragPos = vec3(skinnedPos);

        mat3 normalMatrix = mat3(transpose(inverse(model * skinMatrix)));
        Normal = normalize(normalMatrix * aNormal);

        gl_Position = projection * view * skinnedPos;
    }
    else
    {
        vec4 pos = model * vec4(aPos, 1.0);
        FragPos = vec3(pos);

        mat3 normalMatrix = mat3(transpose(inverse(model)));
        Normal = normalize(normalMatrix * aNormal);

        gl_Position = projection * view * pos;
    }
}
