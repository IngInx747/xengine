#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;
uniform sampler2D TexBloom1;
uniform sampler2D TexBloom2;
uniform sampler2D TexBloom3;
uniform sampler2D TexBloom4;

void main()
{
    vec3 color = texture(TexSrc, TexCoord).rgb;

    const float strength = 0.5;
    vec3 bloom1 = texture(TexBloom1, TexCoord).rgb * strength * 1.00;
    vec3 bloom2 = texture(TexBloom2, TexCoord).rgb * strength * 0.75;
    vec3 bloom3 = texture(TexBloom3, TexCoord).rgb * strength * 0.50;
    vec3 bloom4 = texture(TexBloom4, TexCoord).rgb * strength * 0.25;
    color += bloom1;
    color += bloom2;
    color += bloom3;
    color += bloom4;
    
    FragColor = vec4(color, 1.0);
}