#version 460 core
layout (location = 0) in vec3 inPos;

out vec3 vUV;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vUV = inPos;
	vec4 pos = projection * view * vec4(inPos, 1.0);
	gl_Position = pos.xyww;
}