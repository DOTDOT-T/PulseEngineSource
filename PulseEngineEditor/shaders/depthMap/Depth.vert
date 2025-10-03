#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;

// Pseudo-random generator based on position
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Function to create fake shadow depth
float fakeShadowDepth(vec3 pos) {
    // Scale and offset to get varied shadows
    float pattern = sin(pos.x * 10.0) * cos(pos.y * 10.0);
    pattern += rand(pos.xy * 3.1415); // add randomness
    return 0.5 + 0.5 * pattern;       // normalize to [0,1]
}

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    // Create fake shadow depth independent of light
    float depth = fakeShadowDepth(worldPos.xyz);

    // Output as depth in clip space
    gl_Position = vec4(worldPos.xy, depth, 1.0);
}
