#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;

// motion blur
out vec4 currClipSpacePos;
out vec4 prevClipSpacePos;

#include ../common/uniforms.glsl

uniform mat4 model;
uniform mat4 prevModel;

float time;

void main()
{
	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4(aPosition, 1.0));
        
    vec3 N = normalize(mat3(model) * aNormal);
    Normal = N;
    
    vec3 T = normalize(mat3(model) * aTangent);
    T = normalize(T - dot(N, T) * N);
    // vec3 B = cross(N, T);
    vec3 B = normalize(mat3(model) * aBitangent);

    // TBN must form a right handed coord system.
    // Some models have symetric UVs. Check and fix.
    if (dot(cross(N, T), B) < 0.0)
        T = T * -1.0;
    
    TBN = mat3(T, B, N);
    
    // motion blur
    currClipSpacePos = viewProjection * model * vec4(aPosition, 1.0);
    prevClipSpacePos = prevViewProjection * prevModel * vec4(aPosition, 1.0);
	
	gl_Position =  projection * view * vec4(FragPos, 1.0);
}