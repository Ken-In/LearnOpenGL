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

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 worldPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(cameraPos - WorldPos.xyz);
    vec3 normal = normalize(Normal);
    
    vec3 result;
    //vec3 result = CalcDirLight(dirLight, normal, viewDir);

    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], normal, WorldPos.xyz, viewDir);

    result += CalcSpotLight(spotLight, normal, WorldPos.xyz, viewDir);

    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, Texcoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, Texcoord));

    vec3 halfvec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfvec, normal), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 worldPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - worldPos);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, Texcoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, Texcoord));

    vec3 halfvec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfvec, normal), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    
    float distance = length(light.position - worldPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 worldPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - worldPos);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    if(theta > light.outerCutOff)
    {
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, Texcoord));

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, Texcoord));

        vec3 halfvec = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfvec, normal),0.0), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    
        vec3 result = ambient + diffuse * intensity + specular * intensity;
        return result;
    }
    else
    {
        return light.ambient * vec3(texture(material.diffuse, Texcoord));
    }
}
