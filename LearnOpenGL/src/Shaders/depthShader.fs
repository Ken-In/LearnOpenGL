#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));//��任������    
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // Ϊ����ʾ���� far
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}