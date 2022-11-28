#version 460 core
in vec3 vNormal;
in vec3 vPos;
in vec2 vUV;
in vec3 vTangentLight;
in vec3 vTangentView;
in vec3 vTangentFrag;

struct Material {
	sampler2D albedo;
	vec4 albedoFactor;
	float metallic;
	float roughtness;
	sampler2D ao;
	float occulsionStrength;
	sampler2D normal;
	float normalScale;
	sampler2D emission;
	vec3 emissiveFactor;
};

uniform Material material;
uniform vec3 sun_position;
uniform vec3 sun_color;
uniform vec3 view_position;

out vec4 color;

void main()
{
	vec3 normal = texture(material.normal, vUV).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 albedo = texture(material.albedo, vUV).rgb;

	//ambient
	vec3 ambient = 0.1 * albedo;

	//diffuse
	vec3 lightDir = normalize(vTangentLight - vTangentFrag);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * albedo;

	//specular
	vec3 viewDir = normalize(vTangentView - vTangentFrag);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;

	vec3 emission = texture(material.emission, vUV).rgb;

	color = vec4(ambient + diffuse + specular + emission, 1.0);
}