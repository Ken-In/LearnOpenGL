#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

const float gamma = 2.2;

void main()
{ 
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    float kernel[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );

    //vec3 sampleTex[9];
    //for(int i = 0; i < 9; i++)
    //{
    //    sampleTex[i] = texture(screenTexture, TexCoords.st + offsets[i]).rgb;
    //}
    //vec3 col = vec3(0.0);
    //for(int i = 0; i < 9; i++)
    //    col += sampleTex[i] * kernel[i];

    //FragColor = vec4(col, 1.0f);
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));//gamma矫正
}