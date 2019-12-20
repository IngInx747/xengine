#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/shadows.glsl
#include ../common/uniforms.glsl

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPbrParam;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D lightShadowMap;
uniform mat4 lightShadowViewProjection;

uniform int UseSSAO;
uniform sampler2D TexSSAO;

void main()
{
    vec3 worldPos   = texture(gPosition, TexCoord).xyz;
    vec3 normal     = texture(gNormal, TexCoord).xyz;
    vec3 albedo     = texture(gAlbedo, TexCoord).rgb;
    vec3 pbrParam   = texture(gPbrParam, TexCoord).rgb;
    float metallic  = pbrParam.r;
    float roughness = pbrParam.g;
    float ao        = pbrParam.b;
    
    if (UseSSAO == 1)
    {
        ao *= texture(TexSSAO, TexCoord).r;
    }

    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos.xyz - worldPos); // view-space camera is (0, 0, 0): (0, 0, 0) - viewPos = -viewPos
    vec3 L = normalize(-lightDir);
    vec3 H = normalize(V + L);
	              
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
          
    // calculate light radiance    
    vec3 radiance = lightColor;        
    
    // light shadow
    vec4 fragPosLightSpace = lightShadowViewProjection * vec4(worldPos, 1.0);
    float shadow = ShadowFactor(lightShadowMap, fragPosLightSpace, N, L);
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometryGGX(max(dot(N, V), 0.0), max(dot(N, L), 0.0), roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow); 
        
    FragColor.rgb = Lo * ao;
    // FragColor.rgb = vec3(shadow);
    FragColor.a = 1.0;
}