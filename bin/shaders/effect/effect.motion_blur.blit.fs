#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;
uniform sampler2D TexOrg;

void main()
{
    /**
    Note: Motion Mixer
    To overcome the con of SSMB not blurring dynamic object, one can capture motion
    information of moving objects in other passes and attach to previous result.
    Notice that one can attach more than 1 motion texture.
    */

    vec3 newMotion = texture(TexSrc, TexCoord).rgb;
    vec3 oldMotion = texture(TexOrg, TexCoord).rgb;

    vec3 motion = oldMotion + newMotion;

    FragColor = vec4(motion, 1.0);
}