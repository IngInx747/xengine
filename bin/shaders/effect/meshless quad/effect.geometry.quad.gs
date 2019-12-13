#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

out vec2 TexCoord;

void main()
{
    // (3) 
    //  |  *
    //  |    *
    //  |______*
    //  |      | *
    //  |      |   *
    // (1)_____|_____(2)
    //
    // (1): pos=( 0, 0), uv=(0,0)
    // (2): pos=( 3,-1), uv=(2,0)
    // (3): pos=(-1, 3), uv=(0,2)

    gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = vec4(3.0, -1.0, 0.0, 1.0);
    TexCoord = vec2(2.0, 0.0);
    EmitVertex();

    gl_Position = vec4(-1.0, 3.0, 0.0, 1.0);
    TexCoord = vec2(0.0, 2.0);
    EmitVertex();

    EndPrimitive();
}