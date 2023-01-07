#version 330 core
out vec4 FragColor;

in vec3 ourColor;
uniform vec4 timeColor;


void main()
{
    //FragColor = vec4(ourColor, 1.0f);
    FragColor = timeColor;

}