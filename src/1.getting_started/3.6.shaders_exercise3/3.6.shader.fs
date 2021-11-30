#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 ourPosition;

void main()
{
    // color value will get clamped between [0,1]
    // aPos.z ==0, so if (aPos.x <=0 && aPos.y<=0) will get black color.
    FragColor = vec4(ourPosition, 1.0f);
}