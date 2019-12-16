#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gPbrParam;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/reflections.glsl
#include ../common/uniforms.glsl

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const float rsFalloff = 3.0; // reflection Specular Falloff Exponent
const int numBinarySearchSteps = 5;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 hash(vec3 a);

void main()
{
    vec3 pbrParam = texture(gPbrParam, TexCoord).rgb;
    float metallic = pbrParam.r;

    if (metallic < 0.01)
        discard;

    vec3 worldPos = texture(gPosition, TexCoord).xyz;
    vec3 normal = texture(gNormal, TexCoord).xyz;
    vec3 albedo = texture(TexSrc, TexCoord).rgb;

    vec3 V = normalize(camPos.xyz - worldPos);
    vec3 N = normalize(normal);

    vec4 screenSpacePos = viewProjection * vec4(worldPos, 1.0);
    float spec = screenSpacePos.z / screenSpacePos.w;

    vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);
    vec3 F = FresnelSchlick(max(dot(N, V), 0.0), F0);

    // Reflection vector
    vec3 R = normalize(reflect(V, N));


    vec3 hitPos = worldPos;
    float dDepth;
 
    vec3 jitt = mix(vec3(0.0), vec3(hash(worldPos)), spec);
    vec4 coords = RayMarch((vec3(jitt) + R * max(minRayStep, -worldPos.z)), hitPos, dDepth);
 
 
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));


    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(metallic, rsFalloff) * screenEdgefactor * (-R.z);

    // Get color
    vec3 reflectColor = texture(TexSrc, coords.xy).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * F;  

    FragColor = vec4(reflectColor, 1.0);
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = viewProjection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPosition, projectedCoord.xy).z;
 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;

        if (dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

        projectedCoord = viewProjection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

    dir *= step;
 
 
    float depth;
    int steps;
    vec4 projectedCoord;

 
    for (int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = viewProjection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPosition, projectedCoord.xy).z;
        if (depth > 1000.0)
            continue;
 
        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2)
        {
            if (dDepth <= 0.0)
            {   
                vec4 Result;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
        }
        
        steps++;
    }
    
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}