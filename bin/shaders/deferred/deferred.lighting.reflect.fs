#version 430 core

out vec4 FragColor;

noperspective in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gPbrParam;
uniform sampler2D LastImage;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/reflections.glsl
#include ../common/uniforms.glsl

vec2 WorldPositionToScreenCoord(vec3 world_pos)
{
    vec4 proj_pos = viewProjection * vec4(world_pos, 1.0);
    vec2 screen_coord = proj_pos.xy / proj_pos.w;
    screen_coord = screen_coord * 0.5 + 0.5;
    return screen_coord;
}

float ViewSpaceDepth(vec3 world_pos)
{
    vec3 view_pos = vec3(view * vec4(world_pos, 1.0));
    return view_pos.z;
}

float ViewSpaceDeltaDepth(vec3 world_pos_to, vec3 world_pos_from)
{
    vec3 view_pos_delta = vec3(view * vec4(world_pos_to - world_pos_from, 0.0));
    return view_pos_delta.z;
}

vec3 WorldSpaceHash(vec3 a)
{
    a = fract(a * vec3(.8, .8, .8));
    a += dot(a, a.yxz + 19.19);
    return fract((a.xxy + a.yxx)*a.zyx);
}

const int RT_MaxNumBinSteps = 50;
const float RT_Epsilon = 1e-3;

float RayTraceBinarySearch(vec3 ray_org, vec3 ray_dir, float ray_len_a, float ray_len_b)
{
    float ray_len = 0;

    for (int i = 0; i < RT_MaxNumBinSteps; i++)
    {
        ray_len = (ray_len_a + ray_len_b) * 0.5;

        // apply the ray march
        vec3 ray_front = ray_org + ray_dir * ray_len;

        // project ray frontier in world space onto screen UV
        vec2 screen_coord = WorldPositionToScreenCoord(ray_front);

        // get position in world space which shares the UV of the hit point on screen
        vec3 world_pos = texture(gPosition, screen_coord).xyz;

        // get delta depth value between ray frontier and position
        float dz = ViewSpaceDeltaDepth(ray_front, world_pos);

        // if delta depth value is within the threshold, search passes
        if (abs(dz) < RT_Epsilon)
            break;
        else if (dz > 0.0)
            ray_len_a = ray_len;
        else
            ray_len_b = ray_len;
    }

    return ray_len;
}

const float RT_MaxNumMarchSteps = 200;

vec2 RayTraceMarch(vec3 ray_org, vec3 ray_dir, float march_step)
{
    float ray_len_curr = 0; // current march length
    float ray_len_last = 0; // last march length

    for (int i = 0; i < RT_MaxNumMarchSteps; i++)
    {
        ray_len_last = ray_len_curr;
        ray_len_curr += march_step;
        
        // apply the ray march
        vec3 ray_front = ray_org + ray_dir * ray_len_curr;

        // project ray frontier in world space onto screen UV
        vec2 screen_coord = WorldPositionToScreenCoord(ray_front);

        // if screen UV exceeds screen, search fails
        if (abs(screen_coord.x) >= 1.0 || abs(screen_coord.y) >= 1.0) return vec2(0, 0);

        // sample the g-buffer with the UV
        vec4 g_position = texture(gPosition, screen_coord);

        // if the position is invalid, search fails
        if (g_position.a == 0.0) return vec2(0, 0);

        // get position in world space which shares the UV of the hit point on screen
        vec3 world_pos = g_position.xyz;

        // get delta depth value between ray frontier and position
        float dz = ViewSpaceDeltaDepth(ray_front, world_pos);

        // if sign of dZ flips, a potential hit point is found
        if (dz < 0.0) return vec2(ray_len_last, ray_len_curr);
    }

    return vec2(0, 0);
}

const float rsFalloff = 3.0; // reflection Specular Falloff Exponent

void main()
{
    vec3 pbrParam = texture(gPbrParam, TexCoord).rgb;
    float metallic = pbrParam.r;
    float roughness = pbrParam.g;
    if (metallic < 0.01) discard;

    vec3 worldPos = texture(gPosition, TexCoord).xyz;
    vec3 worldNor = texture(gNormal, TexCoord).xyz;
    vec3 lastImg = texture(LastImage, TexCoord).rgb;

    vec3 V = normalize(camPos.xyz - worldPos);
    vec3 N = normalize(worldNor);
    vec3 R = normalize(reflect(-V, N));

    // Noise
    vec4 screenSpacePos = viewProjection * vec4(worldPos, 1.0);
    float spec = screenSpacePos.z / screenSpacePos.w;
    vec3 jitt = mix(vec3(0.0), vec3(WorldSpaceHash(worldPos)), spec);
    float noise_factor = 0.01 + roughness * 0.05; // basic + enhancement

    // Ray tracing
    vec3 ray_org = worldPos;
    vec3 ray_dir = normalize(R + jitt * noise_factor);
    float march_step = 2.0;

    vec2 ray_len_range = RayTraceMarch(ray_org, ray_dir, march_step); // rough search

    float ray_len_left = ray_len_range.x;
    float ray_len_right = ray_len_range.y;
    if (ray_len_left >= ray_len_right) discard; // tracing failed

    float ray_len = RayTraceBinarySearch(ray_org, ray_dir, ray_len_left, ray_len_right); // fine search

    vec3 ray_front = ray_org + ray_dir * ray_len;
    vec2 pixel_hit_coord = WorldPositionToScreenCoord(ray_front);

    // PBR
    vec3 F0 = vec3(0.04); 
	F0 = mix(F0, lastImg, metallic);
    vec3 fresnel_factor = FresnelSchlick(max(dot(N, V), 0.0), F0);

    // Effect
    vec2 delta_coord = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - pixel_hit_coord));
    float screen_edge_factor = clamp(1.0 - (delta_coord.x + delta_coord.y), 0.0, 1.0);
    float reflect_factor = pow(metallic, rsFalloff) * screen_edge_factor;

    // Get color
    vec3 reflectColor = texture(LastImage, pixel_hit_coord).rgb * fresnel_factor * clamp(reflect_factor, 0.0, 1.0);

    FragColor = vec4(reflectColor, 1.0);
}