#version 430

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

out vec2 TexCoord;

#include ../common/uniforms.glsl

uniform float gBillboardSize;

void main()
{
    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec3 forward = normalize(Pos - camPos.xyz);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, forward) * gBillboardSize;

    Pos -= right;
    gl_Position = viewProjection * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    Pos.y += gBillboardSize;                                                        
    gl_Position = viewProjection * vec4(Pos, 1.0);                                             
    TexCoord = vec2(0.0, 1.0);                                                      
    EmitVertex();                                                                   

    Pos.y -= gBillboardSize;
    Pos += right;
    gl_Position = viewProjection * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    Pos.y += gBillboardSize;
    gl_Position = viewProjection * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}