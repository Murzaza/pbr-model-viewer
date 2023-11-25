#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool has_tangent;

out vec3 vNormal;
out vec3 vPos;
out vec2 vUV;
out mat3 vTBN;

mat3 generateTBN(vec3 normal)
{
	vec3 bitangent = vec3(0.0, 1.0, 0.0);

	float NdotUp = dot(normal, vec3(0.0, 1.0, 0.0));
	float epsilon = 0.0000001;
	if (1.0 - abs(NdotUp) <= epsilon)
	{
		// Sampling +Y or -Y, so we need a more robust bitangent
		if (NdotUp > 0.0)
		{
			bitangent = vec3(0.0, 0.0, 1.0);
		}
		else
		{
			bitangent = vec3(0.0, 0.0, -1.0);
		}
	}

	vec3 tangent = normalize(cross(bitangent, normal));
	bitangent = cross(normal, tangent);

	return mat3(tangent, bitangent, normal);
}

void main()
{
	gl_Position = projection * view * model * vec4(inPos, 1.0);
	vPos = vec3(model * vec4(inPos, 1.0));
	vNormal = normalize(mat3(projection * view * model) * inNormal);
	vUV = inUV;

	mat3 TBN;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	if (has_tangent)
	{
		vec3 T = normalize(normalMatrix * inTangent);
		vec3 N = normalize(normalMatrix * inNormal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		TBN = transpose(mat3(T, B, N));
	}
	else
	{
		TBN = generateTBN(normalize(normalMatrix * inNormal));
	}
	vTBN = TBN;
}