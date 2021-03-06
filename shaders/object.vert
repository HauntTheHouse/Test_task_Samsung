#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    FragPos = vec3(view * model * vec4(aPosition, 1.0f));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    FragPosLightSpace = lightSpaceMatrix * model * vec4(aPosition, 1.0f);
    TexCoords = aTexCoords;
}