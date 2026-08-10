//FRAGMENT SHADER
#version 330 core

out vec4 FragColor;

in vec4 vertexColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vertexColor * vec4(lightColor, 1.0);
}