#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float MotionScale;
uniform int MotionSamples;
uniform sampler2D TexSrc;
uniform sampler2D TexMotion;

void main()
{
    vec3 color = texture(TexSrc, TexCoord).rgb;
    vec2 motion  = texture(TexMotion, TexCoord).rg * MotionScale;

    vec3 avgColor = color;

    for(int i = 0; i < MotionSamples; ++i)
    {
        vec2 offset = motion * (float(i) / float(MotionSamples - 1) - 0.5);
        avgColor += texture(TexSrc, TexCoord + offset).rgb;
    }
    
    color = avgColor / float(MotionSamples);

    FragColor = vec4(color, 1.0);
}