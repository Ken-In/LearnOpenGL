#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 aNormal;

out vec3 Color;
out vec2 Texcoord;
out vec4 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    WorldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * WorldPos;
    Color = aColor;
    Texcoord = aTexcoord;
    Normal = mat3(transpose(inverse(model))) * aNormal;
}