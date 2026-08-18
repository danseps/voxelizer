//FRAGMENT SHADER
#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec4 vertexColor;
in vec3 normal;
in vec2 texCoord;
in float ao;

uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    vec3 norm = normalize(normal); 

    float upFactor = norm.y * 0.5 + 0.5; // Normalize the y component to [0, 1]

    // ambient
    /*float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;*/
    vec3 skyColor = vec3(0.4, 0.42, 0.5); 
    vec3 groundColor = vec3(0.3, 0.25, 0.22); 
    vec3 ambient = mix(groundColor, skyColor, upFactor) * lightColor; 

    // diffuse
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
    vec3 lighting = (ambient + diffuse) * ao;
    FragColor = texture(texture1, texCoord) * vec4(lighting, 1.0);
}