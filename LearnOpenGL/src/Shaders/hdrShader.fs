#version 330 core
out vec4 color;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // Reinhardɫ��ӳ��
    //vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // �ع�ɫ��ӳ��
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // GammaУ��
    //mapped = pow(mapped, vec3(1.0 / gamma));
    
    color = vec4(mapped, 1.0);
}