//VERTEX SHADER
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord; //TODO:

out vec3 fragPos;
out vec4 vertexColor;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    vertexColor = aCol;
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space TODO: on CPU, send through uniform
    texCoord = aTexCoord;
}