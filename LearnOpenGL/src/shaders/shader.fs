#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 Texcoord;

uniform float mixValue;
uniform vec4 timeColor;
uniform sampler2D textureContainer;
uniform sampler2D textureWall;
uniform sampler2D textureFace;
uniform sampler2D textureKeqing;


void main()
{
    //FragColor = vec4(ourColor, 1.0f);
    //FragColor = timeColor;
    //vec2 convenseTex = vec2( 1.0 - Texcoord.x, Texcoord.y);
    FragColor = mix(texture(textureFace, Texcoord), texture(textureKeqing, Texcoord), mixValue);
}