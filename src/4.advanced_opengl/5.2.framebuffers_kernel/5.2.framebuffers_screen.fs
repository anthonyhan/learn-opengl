#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

const vec2 offsets[9] = vec2[](
           vec2(-offset,  offset), // LeftTop
           vec2( 0.0f,    offset), // Top
           vec2( offset,  offset), // RightTop
           vec2(-offset,  0.0f),   // Left
           vec2( 0.0f,    0.0f),   // Center
           vec2( offset,  0.0f),   // Right
           vec2(-offset, -offset), // LeftBottom
           vec2( 0.0f,   -offset), // Bottom
           vec2( offset, -offset)  // RightBottom
       );


float[9] get_kernel_sharpen()
{
    return float[](
           -1, -1, -1,
           -1,  9, -1,
           -1, -1, -1
       );
}

float[9] get_kernel_blur()
{
    return float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
}

float[9] get_kernel_edge()
{
    return float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );
}

void main()
{
    float kernel[9] = get_kernel_sharpen();
    vec3 sampleTex[9];

    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}
