#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 sun_position;
uniform vec3 view_position;

out vec3 vNormal;
out vec3 vPos;
out vec2 vUV;
out vec3 vTangentLight;
out vec3 vTangentView;
out vec3 vTangentFrag;

void main()
{
	gl_Position = projection * view * model * vec4(inPos, 1.0);
	vPos = vec3(model * vec4(inPos, 1.0));
	vNormal = normalize(mat3(projection * view * model) * inNormal);
	vUV = inUV;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * inTangent);
	vec3 N = normalize(normalMatrix * inNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vTangentLight = TBN * sun_position;
	vTangentView = TBN * view_position;
	vTangentFrag = TBN * vPos;
}