#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out vec2 Texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	Texcoords = aTexcoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}