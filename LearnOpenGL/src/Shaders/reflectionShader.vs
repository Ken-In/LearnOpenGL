#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;

out VS_OUT
{
	vec2 Texcoords;
	vec3 Normal;
	vec3 Position;
} vs_out;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;


void main()
{
	vs_out.Texcoords = aTexcoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.Position = vec3(model * vec4(aPos, 1.0f));
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_PointSize = gl_Position.z;
}