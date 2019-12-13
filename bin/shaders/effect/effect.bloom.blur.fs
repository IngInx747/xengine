#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;
uniform bool bHorizontal;

const float offsets[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float weights[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main()
{             
    vec2 texSize = textureSize(TexSrc, 0); 
    vec2 tex_offset = (1.0 / texSize); // gets size of single texel
    vec3 result = texture(TexSrc, TexCoord).rgb * weights[0]; // current fragment's contribution

    if (bHorizontal)
    {
        for (int i = 1; i < 3; ++i)
        {
            result += texture(TexSrc, TexCoord + vec2(offsets[i] / texSize.x, 0.0)).rgb * weights[i];
            result += texture(TexSrc, TexCoord - vec2(offsets[i] / texSize.x, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 3; ++i)
        {
            result += texture(TexSrc, TexCoord + vec2(0.0, offsets[i] / texSize.y)).rgb * weights[i];
            result += texture(TexSrc, TexCoord - vec2(0.0, offsets[i] / texSize.y)).rgb * weights[i];
        }
    }

    FragColor = vec4(result, 1.0);
}