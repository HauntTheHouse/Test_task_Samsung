#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

uniform sampler2D objectTexture;
uniform sampler2D shadowMap;
uniform vec3 materialColor;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 fragPos);
float shadowCalculation(vec4 fragPosLightSpace);

void main()
{
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    vec3 norm = normalize(Normal);

    color += calcDirLight(dirLight, norm, FragPos);

    FragColor = vec4(color, 1.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 fragPos)
{
    vec3 texColor = vec3(texture(objectTexture, TexCoords));

    vec3 ambient = light.ambient * texColor;

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;


    vec3 viewDir = normalize(-fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
    vec3 specular = light.specular * spec * texColor;

    float shadow = shadowCalculation(FragPosLightSpace);

    return (ambient + (1.0f - shadow) * (diffuse + specular)) * materialColor;
}

float shadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.0025;

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depthPCF = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > depthPCF ? 1.0f : 0.0f;
        }
    }
    shadow /= 9.0f;
//    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}