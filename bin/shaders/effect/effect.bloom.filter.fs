#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;

void main()
{
    vec3 color = texture(TexSrc, TexCoord).rgb;
    // float grayscale = dot(color, vec3(0.299, 0.587, 0.114));
    // color = smoothstep(1.0, 4.0, grayscale) * color;
    // FragColor.rgb = color;
    FragColor.rgb = 0.1 * color;
    FragColor.a = 1.0;
}