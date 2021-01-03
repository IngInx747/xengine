#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;

// post-process effect toggles
uniform int Vignette;
uniform int Sepia;

// sepia
const vec3 sepiaColor = vec3(1.2, 1.0, 0.8);

// vignette

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 aces(vec3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(TexSrc, TexCoord).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    vec2 texelSize = 1.0 / textureSize(TexSrc, 0).xy;    

    // HDR tonemapping
    const float exposure = 1.0;
    color *= exposure;
    // color = aces(color);
    color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     
    
    if (Sepia == 1)
    {
        color = mix(color, grayscale * sepiaColor, 0.7);
    }

    if (Vignette == 1)
    {
        const float strength = 10.0;
        const float power = 0.1;
        vec2 tuv = TexCoord * (vec2(1.0) - TexCoord.yx);
        float vign = tuv.x*tuv.y * strength;
        vign = pow(vign, power);
        color *= vign;
    }

    FragColor = vec4(color, 1.0);
}