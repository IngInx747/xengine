#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec4 ScreenPos;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/uniforms.glsl

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPbrParam;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightRadius;

void main()
{
    vec2 TexCoord = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;
    
    vec3 worldPos   = texture(gPosition, TexCoord).xyz;
    vec3 normal     = texture(gNormal, TexCoord).xyz;
    vec3 albedo     = texture(gAlbedo, TexCoord).rgb;
    vec3 pbrParam   = texture(gPbrParam, TexCoord).rgb;
    float metallic  = pbrParam.r;
    float roughness = pbrParam.g;
    float ao        = pbrParam.b;

    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos.xyz - worldPos); 
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);     
	              
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
          
    // calculate light radiance (based of UE4's light attenuation model)   
    float distance = length(worldPos - lightPos);
    float attenuation = pow(clamp(1.0 - pow(distance / lightRadius, 1.0), 0.0, 1.0), 2.0) / (distance * distance + 1.0);
    // float attenuation = max(0.95 - length(worldPos - lightPos) / lightRadius, 0.0);
    vec3 radiance = lightColor * attenuation;        
        
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometryGGX(max(dot(N, V), 0.0), max(dot(N, L), 0.0), roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
    //vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL; 
    
    FragColor.rgb = Lo;
    FragColor.a = 1.0;
}