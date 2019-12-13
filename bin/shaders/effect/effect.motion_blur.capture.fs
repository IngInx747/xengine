#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 camPos;
uniform vec3 camForward;
uniform vec3 camUp;
uniform vec3 camRight;
uniform mat4 currView;
uniform mat4 prevView;
uniform mat4 projection;
uniform sampler2D gPosition;

const float skyboxDist = 10.0;

void main()
{
    /**
    Note: Screen Space Motion Blur
    The shader only takes care of camera movement into account, i.e., blurring effect
    caused by camera motion and view changing only.
    Dynamic object like bouncing ball or rotating torus will not cause motion blurring
    on the screen, as a previous world space transform matrix is required.
    If dynamic object in the scene need be considered, this shader cannot help. One
    capable approach is to record objects' motion information in a deferred pass.
    */

    vec4 position = texture(gPosition, TexCoord);
    vec3 worldPos = position.xyz;

    if (position.w == 0.0)
    {
        worldPos = camPos;
        worldPos += (camForward + camUp * FragPos.y + camRight * FragPos.x) * skyboxDist;
    }

    FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec4 currClipSpacePos = projection * currView * vec4(worldPos, 1.0);
    vec3 currScreenSpacePos = currClipSpacePos.xyz / currClipSpacePos.w;

    vec4 prevClipSpacePos = projection * prevView * vec4(worldPos, 1.0);
    vec3 prevScreenSpacePos = prevClipSpacePos.xyz / prevClipSpacePos.w;

    vec3 motion = currScreenSpacePos - prevScreenSpacePos;

    FragColor.rgb = motion;
}