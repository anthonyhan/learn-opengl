#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

const int GBUFFER_DEFAULT = 0;
const int GBUFFER_POSITION = 1;
const int GBUFFER_NORMAL = 2;
const int GBUFFER_ALBEDO = 3;
const int GBUFFER_SPECULAR = 4;

uniform int gbuffer_display_mode = 0;
uniform bool light_attenuation = true;
uniform float exposure = 1.0f;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

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
            FragColor = vec4(Specular,Specular,Specular, 1.0);
        break;
        default:
        {
            // calculate lighting
            vec3 lighting = Diffuse * 0.1f;
            
            for(int i = 0; i <  NR_LIGHTS; ++i)
            {
                float distance = length(lights[i].Position - FragPos);
                if(distance < lights[i].Radius)
                {
                    vec3 viewDir = normalize(viewPos - FragPos);

                    // diffuse
                    vec3 lightDir = normalize(lights[i].Position - FragPos);
                    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

                    // specular (blinn-phong)
                    vec3 halfwayDir = normalize(lightDir + viewDir);
                    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
                    vec3 specular = lights[i].Color * spec * Specular;

                    if(light_attenuation)
                    {
                        // attenuation
                        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
                        diffuse *= attenuation;
                        specular *= attenuation;
                    }
                    
                    lighting += diffuse + specular;  
                }
            }
            //FragColor = vec4(lighting, 1.0);

            // exposure tone mapping
            vec3 mapped = vec3(1.0) - exp(-lighting * exposure);

            // gamma correction
            mapped = pow(mapped, vec3(1.0 / 2.2f));

            FragColor = vec4(mapped, 1.0);

            break;
        }
    }


}