#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TexSrc;

void main()
{
    /**
    Note: Motion Mixer
    To overcome the con of SSMB not blurring dynamic object, one can capture motion
    information of moving objects in other passes and attach to previous result.
    Notice that one can attach more than 1 motion texture.

    OGL Note:
    To add new pixel value to current pixel value instead of simply replacing, the
    OpenGL blend trick is used here. Specifically the final pixel value should be:
    Pf = Ps * fs + Pd * fd (P is vec4 and f is float) where fs and fd are both 1.
    In practice one should enable blend and then set both sfactor and dfactor GL_ONE.
    */

    vec2 motion = texture(TexSrc, TexCoord).rg;

    FragColor = vec4(motion, 0.0, 1.0);
}