#version 330 core

layout (location = 0) out vec4 gPosition; // RBG: position, A: discard flag
layout (location = 1) out vec4 gNormal; // RBG: normal, A: NA
layout (location = 2) out vec4 gAlbedo; // RGB: color, A: NA
layout (location = 3) out vec4 gPbrParam; // R: Metallic, G: Roughness, B: Ambient Occlusion, A: NA
layout (location = 4) out vec4 gMotion; // RGB: motion, A: NA

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

// motion blur
in vec4 currClipSpacePos;
in vec4 prevClipSpacePos;

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

void main()
{
    // world space position
    gPosition.rgb = FragPos;
    gPosition.a = 1.0;

    // world space normal
    vec3 N = Normal;
#ifdef MESH_TBN
    N = texture(TexNormal, TexCoord).rgb;    
    N = normalize(N * 2.0 - 1.0);
    N = normalize(TBN * N);
#endif

    gNormal.rgb = normalize(N);
    gNormal.a = 1.0;

    // color
    gAlbedo.rgb = texture(TexAlbedo, TexCoord).rgb;
    gAlbedo.a = 1.0;

    // pbr parameters
    gPbrParam.r = texture(TexMetallic, TexCoord).r;
    gPbrParam.g = texture(TexRoughness, TexCoord).r;
    gPbrParam.b = texture(TexAO, TexCoord).r;
    gPbrParam.a = 1.0;

    // motion blur
    vec2 currClipSpace = currClipSpacePos.xy / currClipSpacePos.w;
    vec2 prevClipSpace = prevClipSpacePos.xy / prevClipSpacePos.w;
    gMotion = vec4(0, 0, 0, 1);
    gMotion.xy = currClipSpace - prevClipSpace;
}  