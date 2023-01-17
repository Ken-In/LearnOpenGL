#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
	vec2 TexCoords;
	vec3 Normal;
	vec3 Position;
} gs_in[];

out vec2 TexCoords;
out vec3 Normal;
out vec3 Position;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
   return normalize(gs_in[0].Normal + gs_in[1].Normal + gs_in[2].Normal);
}

void main() {
	vec3 normal = GetNormal();

    //gl_Position = explode(gl_in[0].gl_Position, normal);
    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].TexCoords;
    Normal = gs_in[0].Normal;
    Position = gs_in[0].Position;
    EmitVertex();

    //gl_Position = explode(gl_in[1].gl_Position, normal);
    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].TexCoords;
    Normal = gs_in[1].Normal;
    Position = gs_in[1].Position;
    EmitVertex();

    //gl_Position = explode(gl_in[2].gl_Position, normal);
    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].TexCoords;
    Normal = gs_in[2].Normal;
    Position = gs_in[2].Position;
    EmitVertex();
    EndPrimitive();
}