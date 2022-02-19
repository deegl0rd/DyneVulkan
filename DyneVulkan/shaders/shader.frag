#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

struct PointLight
{
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
	vec4 camerapos;
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor;
	PointLight pointLights[10];
	int numLights;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform Push 
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() 
{
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 surfaceNormal = normalize(fragNormalWorld);

	for(int i = 0; i < ubo.numLights; i++)
	{
		//diffuse light
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;
		vec3 intensity = light.color.xyz * light.color.w;

		float attenuation = 1.0 / dot(directionToLight, directionToLight);
		float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0);

		diffuseLight += intensity * attenuation * cosAngIncidence;

		//specular light
		vec3 viewDir = normalize(ubo.camerapos.xyz - fragPosWorld);
		vec3 reflectDir = reflect(normalize(-directionToLight), surfaceNormal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
		vec3 specular = 0.5f * spec * intensity;

		diffuseLight += specular;
	}

	outColor = texture(texSampler, fragUv) * vec4(diffuseLight * fragColor, 1.0);
}