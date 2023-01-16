#version 330 core
out vec4 FragColor;

in vec2 Texcoords;
in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_reflect1;
uniform samplerCube cubemap;

float ambientFactor = 0.1;
float shininess = 16.0;
float ratio = 1.00 / 1.52;
vec3 lightPos = vec3(-1, 1, -1);

void main()
{    
    vec3 normal = normalize(Normal);
    vec3 baseColor = texture(texture_diffuse1, Texcoords).rgb;
    vec3 lightDir = normalize(lightPos - Position);
    vec3 viewDir = normalize(cameraPos - Position);

    vec3 ambient = ambientFactor * baseColor;

    float diffuseFactor = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diffuseFactor * baseColor;

    vec3 halfvec = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(halfvec, normal), 0.0), shininess);
    vec3 specular = specularFactor * baseColor;

    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    //vec3 R = refract(I, normalize(Normal), ratio);
    vec3 reflectFactor = vec3(texture(texture_reflect1, Texcoords));
    vec3 reflect = 2 * reflectFactor * texture(cubemap, R).rgb;

    FragColor = vec4(ambient + diffuse + specular + reflect, 1.0);
}