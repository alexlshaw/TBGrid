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
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

#define NR_POINT_LIGHTS 4
//layout (std140) uniform LightBlock
//{
//    DirLight dirLight;
//    PointLight pointLights[NR_POINT_LIGHTS];
//};
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos;
uniform sampler2D tex;
uniform vec4 albedo;

vec3 calculateDirectionalLight(DirLight light, vec3 viewDir, vec3 texel)
{
    
    vec3 lightDir = normalize(-light.direction); //s from phong slides
    float diffuse = max(dot(fragment.normal, lightDir), 0.0); //light = IP(s.m)
    vec3 reflectDir = reflect(-lightDir, fragment.normal);   //r from phong slides? almost. GLSL uses -s, while r uses +s
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  //light = IP(v.r)^a or IP(h.m)^a
    //we assume for now that Pa=Pi=Ps, so we can multiply them in later, and get I from light.abc
    vec3 result = (light.ambient * texel) + (diffuse * light.diffuse * texel) + (specular * light.specular * texel);
    return result;
}

vec3 calculatePointLight(PointLight light, vec3 viewDir, vec3 texel)
{
    vec3 lightDir = normalize(light.position - fragment.position);  //s from phong slides
    float diffuse = max(dot(fragment.normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, fragment.normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);  //not very shiny
    //now attenuate the light
    float distance = length(light.position - fragment.position);
    float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * distance * distance));
    vec3 result = (light.ambient * texel) + (((diffuse * light.diffuse * texel) + (specular * light.specular * texel)) * attenuation);
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
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        light += calculatePointLight(pointLights[i], viewDir, texel.xyz);
    }

    finalColor = vec4(light, texel.w);
}