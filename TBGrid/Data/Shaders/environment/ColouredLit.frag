#version 330

precision highp float;

in Fragment
{
    vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec4 color;
} fragment;

out vec4 finalColor;

struct DirLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
};
struct PointLight {    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation;   //constant, linear, quadratic, ignored
    vec4 position;
}; 

#define MAX_LIGHT_COUNT 32
layout (std140) uniform LightBlock
{
    DirLight dirLight;
    PointLight pointLights[MAX_LIGHT_COUNT];
    int lightCount;
};

uniform vec3 viewPos;
uniform sampler2D tex;
uniform vec4 albedo;

vec3 calculateDirectionalLight(DirLight light, vec3 viewDir, vec3 texel)
{
    
    vec3 lightDir = normalize(-light.direction.xyz); //s from phong slides
    float diffuse = max(dot(fragment.normal, lightDir), 0.0); //light = IP(s.m)
    vec3 reflectDir = reflect(-lightDir, fragment.normal);   //r from phong slides? almost. GLSL uses -s, while r uses +s
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  //light = IP(v.r)^a or IP(h.m)^a
    //we assume for now that Pa=Pi=Ps, so we can multiply them in later, and get I from light.abc
    vec3 result = (light.ambient.xyz * texel) + (diffuse * light.diffuse.xyz * texel) + (specular * light.specular.xyz * texel);
    return result;
}

vec3 calculatePointLight(PointLight light, vec3 viewDir, vec3 texel)
{
    vec3 lightDir = normalize(light.position.xyz - fragment.position);  //s from phong slides
    float diffuse = max(dot(fragment.normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, fragment.normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);  //not very shiny
    //now attenuate the light
    float distance = length(light.position.xyz - fragment.position);
    float attenuation = 1.0 / (light.attenuation.x + (light.attenuation.y * distance) + (light.attenuation.z * distance * distance));
    vec3 result = (light.ambient.xyz * texel) + (((diffuse * light.diffuse.xyz * texel) + (specular * light.specular.xyz * texel)) * attenuation);
    return result;
}

void main(void)
{
    //compute the texture colour
	vec4 texel = texture(tex, fragment.texCoords) * fragment.color * albedo;
	if (texel.a < 0.01)
	{
		discard;
	}

    //compute the lighting
    vec3 viewDir = normalize(viewPos - fragment.position);  //v in phong slides - not actually viewDir, since it's vector from point to view
	vec3 light = calculateDirectionalLight(dirLight, viewDir, texel.xyz);
    for (int i = 0; i < lightCount; i++)
    {
        light += calculatePointLight(pointLights[i], viewDir, texel.xyz);
    }

    finalColor = vec4(light, texel.w);
}