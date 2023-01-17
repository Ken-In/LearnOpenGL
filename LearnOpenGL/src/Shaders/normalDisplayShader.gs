#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec3 Normal;
} gs_in[];

const float magnitude = 0.003;

uniform mat4 projection;
uniform mat4 view;

void GenerateLine(int index)
{
    gl_Position = projection * view * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * view * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].Normal, 0.0) * magnitude);
    EmitVertex();
    EndPrimitive();
}

void main() {
	GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}