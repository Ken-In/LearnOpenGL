#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec2 Texcoord;
in vec4 WorldPos;
in vec3 Normal;

uniform sampler2D textureContainer;
uniform sampler2D textureWall;
uniform sampler2D textureFace;
uniform sampler2D textureKeqing;

uniform vec3 cameraPos;
uniform float matrixMove;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    vec3 baseColor = texture(textureWall, Texcoord).xyz;
    vec3 lightDir = normalize(light.position - WorldPos.xyz);
    vec3 viewDir = normalize(cameraPos - WorldPos.xyz);
    vec3 norm = normalize(Normal);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, Texcoord));

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, Texcoord));

    vec3 halfvec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfvec, norm),0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f) + texture(material.emission, vec2(Texcoord.x,Texcoord.y + matrixMove));
}