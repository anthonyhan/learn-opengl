#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform Light light;

const int GBUFFER_DEFAULT = 0;
const int GBUFFER_POSITION = 1;
const int GBUFFER_NORMAL = 2;
const int GBUFFER_ALBEDO = 3;
const int GBUFFER_SPECULAR = 4;
const int GBUFFER_SSAO = 5;

uniform int gbuffer_display_mode = 0;
uniform bool light_attenuation = true;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float Specular = 1.0f;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    switch(gbuffer_display_mode)
    {
        case GBUFFER_POSITION:
            FragColor = vec4(FragPos, 1.0);
        break;
        case GBUFFER_NORMAL:
            FragColor = vec4(Normal, 1.0);
        break;
        case GBUFFER_ALBEDO:
            FragColor = vec4(Diffuse, 1.0);
        break;
        case GBUFFER_SPECULAR:
            FragColor = vec4(vec3(Specular), 1.0);
        break;
        case GBUFFER_SSAO:
            FragColor = vec4(vec3(AmbientOcclusion), 1.0);
        break;
        default:
        {
            // calculate lighting
            vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
            vec3 lighting = ambient;
            vec3 viewDir = normalize( - FragPos); // viewPos is vec3(0.0) in view space

            // diffuse
            vec3 lightDir = normalize(light.Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;

            // specular (blinn-phong)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = light.Color * spec * Specular;

            if(light_attenuation)
            {
                // attenuation
                float distance = length(light.Position - FragPos);
                float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
                diffuse *= attenuation;
                specular *= attenuation;
            }
            
            lighting += diffuse + specular;  

            FragColor = vec4(lighting, 1.0);
        }
        break;
    }
}