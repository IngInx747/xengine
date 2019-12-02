#version 430 core
out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#include common/uniforms.glsl

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

void main()
{
    vec4 albedo = texture(TexAlbedo, TexCoords);
    vec3 N = normalize(Normal) + vec3(1.0); // TODO: normal mapping
                      
    FragColor = vec4(N * 0.5, 1.0);
}
