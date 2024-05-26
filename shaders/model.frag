#version 460 core
in vec3 vNormal;
in vec3 vPos;
in vec2 vUV;
in mat3 vTBN;

struct Material {
	sampler2D albedo;
	vec4 albedoFactor;
	sampler2D pbrMetallicRoughness;
	float metallic;
	float roughness;
	sampler2D ao;
	float occulsionStrength;
	sampler2D normal;
	float normalScale;
	sampler2D emission;
	vec3 emissiveFactor;
};

struct Light {
	vec3 position;
	vec3 color;
};

uniform Material material;
uniform Light lights[4];
uniform vec3 view_position;

const float PI = 3.14159256359;

out vec4 color;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 getTangent()
{
	vec3 tangentNormal = texture(material.normal, vUV).rgb * 2.0 - 1.0;
	return normalize(vTBN * tangentNormal);
}

void main()
{
	vec3 N = getTangent();

	vec3 V = normalize(view_position - vPos);
	vec3 F0 = vec3(0.04);

	vec3 albedo = texture(material.albedo, vUV).rgb;
	float metallic = float(texture(material.pbrMetallicRoughness, vUV).b);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	// Loop over all lights...
	for (int i = 0; i < 4; ++i) {
		//sun radiance
		vec3 light_pos = lights[i].position;
		vec3 light_col = lights[i].color;

		vec3 L = normalize(light_pos - vPos);
		vec3 H = normalize(V + L);
		float dist = distance(light_pos, vPos);
		float attenuation = 1.0 / (dist * dist);
		vec3 radiance = light_col * attenuation;

		// Cook-Torrance BRDF
		float roughness = texture(material.pbrMetallicRoughness, vUV).g;
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
		// End loop over all lights
	}

	float ao = texture(material.ao, vUV).r;
	ao = ao > 0.0 ? ao : 1.0;

	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 wip_color = ambient * Lo;

	// HDR tonemapping
	wip_color = wip_color / (wip_color + vec3(1.0));
	// gamma correction
	wip_color = pow(wip_color, vec3(1.0/2.2));

	// emissive
	vec3 emission = texture(material.emission, vUV).rgb;

	color = vec4(wip_color + emission, 1.0);
}