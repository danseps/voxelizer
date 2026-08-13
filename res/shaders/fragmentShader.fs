//FRAGMENT SHADER
#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec4 vertexColor; // TODO: texture
in vec3 normal;
in vec2 texCoord;

uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    // ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    /*float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;*/

    //FragColor = vertexColor * vec4(ambient + diffuse /*+ specular*/, 1.0);
    FragColor = texture(texture1, texCoord) * vec4(ambient + diffuse /*+ specular*/, 1.0);
}