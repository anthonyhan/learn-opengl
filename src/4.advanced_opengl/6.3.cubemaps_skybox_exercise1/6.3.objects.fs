#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;

uniform vec3 cameraPos;

void main()
{    

    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 refl = texture(texture_ambient1, TexCoords).rgb;
    vec3 result = refl * texture(skybox, R).rgb;

    result += texture(texture_diffuse1, TexCoords).rgb;

    FragColor = vec4(result, 1.0);
}