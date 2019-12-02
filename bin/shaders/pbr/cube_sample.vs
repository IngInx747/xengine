#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 texCoords;
//layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
//uniform mat4 model;

out vec3 WorldPos;

void main()
{
	//vec4 worldPos = model * vec4(aPos, 1.0);
	//WorldPos.xyz = worldPos.xyz;
	WorldPos = aPos;

	mat4 rotView = mat4(mat3(view));
	//vec4 clipPos = projection * rotView * model * worldPos;
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}