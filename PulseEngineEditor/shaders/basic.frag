#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material
uniform vec3 viewPos;
uniform vec3 objectColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

// Light (per point light)
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float attenuation;
    bool castsShadow;
    float farPlane;
    samplerCube depthMap;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
    bool castsShadow;
    sampler2D shadowMap;  // 2D shadow map for directional light
    mat4 lightSpaceMatrix;
};

uniform PointLight pointLights[4]; // Adjust count if needed
uniform int numPointLights;

uniform DirectionalLight dirLight;


float CalculateDirectionalShadow(DirectionalLight light, vec3 fragPos)
{
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = clamp(projCoords, 0.0, 1.0);
    projCoords = projCoords * 0.5 + 0.5;

    // Check if fragment is outside the light's orthographic frustum
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0)
    {
        return 0.0;
    }

    float closestDepth = texture(light.shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}


vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * light.color * 0.5;

    float shadow = light.castsShadow ? CalculateDirectionalShadow(light, fragPos) : 0.0;

    return (1.0 - shadow) * (diffuse + specular);
}


float CalculatePointShadow(PointLight light, vec3 fragPos)
{
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);

    // Sample the depth from the cube map
    float closestDepth = texture(light.depthMap, normalize(fragToLight)).r;

    closestDepth *= light.farPlane;

    // Bias to avoid shadow acne
    float bias = max(0.05 * (1.0 - dot(Normal, normalize(fragToLight))), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if (currentDepth < 0.01)
        return 0.0; // pas d’ombre

    return shadow;
}

vec3 ComputePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.attenuation * (dist * dist));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * light.color * 0.5;

    // Shadow
    float shadow = light.castsShadow ? CalculatePointShadow(light, fragPos) : 0.0;

    vec3 lighting = (1.0 - shadow) * (diffuse + specular);
    return lighting * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightingResult = vec3(0.0);

    // Accumulate lighting from point lights
    for (int i = 0; i < numPointLights; ++i)
    {
        lightingResult += ComputePointLight(pointLights[i], FragPos, norm, viewDir);
    }
    // Add directional light
    lightingResult += ComputeDirectionalLight(dirLight, norm, viewDir, FragPos);

    vec4 albedoColor = texture(albedoMap, TexCoords);

    // Heuristic: consider albedo invalid if color is close to black or alpha near zero
    bool albedoValid = (length(albedoColor.rgb) > 0.01) && (albedoColor.a > 0.01);

    vec3 baseColor = albedoValid ? albedoColor.rgb : objectColor;

    FragColor = vec4(lightingResult, 1.0);
}
