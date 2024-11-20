#version 330

precision highp float;

in Fragment
{
    vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec4 color;
    vec4 positionLightSpace;
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
uniform sampler2D shadowMap;
uniform vec4 albedo;
uniform float shininess;

float calculateShadow(vec3 lightDir)
{
    //bring our coordinates into the 0...1 range we use to sample the shadow map
    vec3 projectionCoords = fragment.positionLightSpace.xyz / fragment.positionLightSpace.w;
    projectionCoords = projectionCoords * 0.5 + 0.5;
    //if its outside of our shadow frustrum's far plane, just say it's not in shadow
    if (projectionCoords.z > 1.0)
    {
        return 0.0;
    }
    //otherwise we do some calculating
    float currentDepth = projectionCoords.z;
    float bias = max(0.0005 * (1.0 - dot(fragment.normal, lightDir)), 0.0005);
    //do percentage-closer filtering (sample all the surrounding points and average them out)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadowMap, projectionCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 calculateDirectionalLight(DirLight light, vec3 viewDir, vec3 texel)
{
    vec3 lightDir = normalize(-light.direction.xyz); //s from phong slides
    float diffuse = max(dot(fragment.normal, lightDir), 0.0); //light = IP(s.m)
    vec3 halfway = normalize(lightDir + viewDir);
    float specular = pow(max(dot(fragment.normal, halfway), 0.0), shininess);  //light = IP(v.r)^a (Phong) or IP(h.m)^a (Blinn)
    //Pa, Pi, Ps coming from texel, and get I from light.abc
    vec3 ambientColor = light.ambient.xyz * texel;
    vec3 diffuseColor = diffuse * light.diffuse.xyz * texel;
    vec3 specularColor = specular * light.specular.xyz * texel;
    float inShadow = calculateShadow(lightDir);
    vec3 result = ambientColor + ((1.0 - inShadow) * (diffuseColor + specularColor));
    return result;
}

vec3 calculatePointLight(PointLight light, vec3 viewDir, vec3 texel)
{
    vec3 lightDir = normalize(light.position.xyz - fragment.position);
    float diffuse = max(dot(fragment.normal, lightDir), 0.0);
    vec3 halfway = normalize(lightDir + viewDir);
    float specular = pow(max(dot(fragment.normal, halfway), 0.0), shininess);
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