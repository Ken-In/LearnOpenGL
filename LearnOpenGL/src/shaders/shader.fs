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

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec3 lightDir = normalize(lightPos - WorldPos.xyz);
    vec3 viewDir = normalize(cameraPos - WorldPos.xyz);
    vec3 norm = normalize(Normal);

    vec3 ambient = light.ambient * material.ambient;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 halfvec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfvec, norm),0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}