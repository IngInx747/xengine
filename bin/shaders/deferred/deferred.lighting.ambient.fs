#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/reflections.glsl
#include ../common/uniforms.glsl

uniform samplerCube envIrradiance;
uniform samplerCube envReflection;
uniform sampler2D   BRDFLUT;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPbrParam;

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

    // lighting data
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos.xyz - worldPos); 
	vec3 R = reflect(-V, N); 
	
	// calculate color/reflectance at normal incidence
    // if dia-electric (like plastic) use F0 as 0.04 and
    // if it's a metal, use their albedo color as F0 (metallic workflow)
	 //kS is equal to Fresnel
	// calculate reflectance w/ (modified for roughness) Fresnel
	vec3 F0 = vec3(0.04); // base reflectance at incident angle for non-metallic (dia-conductor) surfaces 
	F0 = mix(F0, albedo, metallic);
	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
	
	// calculate specular global illumination contribution w/ Epic's split-sum approximation
	const float MAX_REFLECTION_LOD = 5.0;
    vec3 reflectionColor = textureLod(envReflection, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = reflectionColor * (F * envBRDF.x + envBRDF.y);
    
	// for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light) so to preserve this
    // relationship the diffuse component (kD) equals 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// also multiply kD by the inverse metalness s.t. only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals have
    // no diffuse light).
	kD *= 1.0 - metallic;	
	// directly obtain irradiance from irradiance environment map
	vec3 irradiance = texture(envIrradiance, N).rgb;
	vec3 diffuse = albedo * irradiance;
	
	// combine contributions, note that we don't multiply by kS as kS equals
    // the Fresnel value and the specular value was already multipled by Fresnel
    // during the importance sampling.
	vec3 color = kD * diffuse + specular; 
	
	// as the IBL lighting of both diffuse and specular counts as
    //(GI) ambient lighting, we multiply both with the AO component.
    // FragColor.rgb = color.rgb * ao * max(1.0 - dot(invViewRot * N, vec3(0.0, 1.0, 0.0)), 0.05) * 0.5;
    FragColor.rgb = color.rgb * ao;
    FragColor.a = 1.0;
}