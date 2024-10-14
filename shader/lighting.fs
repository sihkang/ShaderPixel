#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform vec3 viewPos;
uniform int blinn;

out vec4 fragColor;

struct Light
{
	vec3 position;
	vec3 direction;
	vec2 cutoff; // 스포트라이트 감쇠
	vec3 attenuation;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

void main()
{
	vec3 texColor = texture(material.diffuse, texCoord).xyz;
	vec3 ambient =  texColor * light.ambient;

	float dist = length(light.position - position); // 광원과 3D 물체 위치간의 거리
	vec3 disPoly = vec3(1.0, dist, dist * dist); // 상수항, 1차항, 2차항.
	float attenuation = 1.0 / dot (disPoly, light.attenuation); // 상수항 1차항 2차항의 계수.
	vec3 lightDir = (light.position - position) / dist;
	
	vec3 result = ambient;
	float theta = dot(lightDir, normalize(-light.direction));

	float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

	if (intensity > 0.0)
	{
		vec3 pixelNorm = normalize(normal);
		float diff = max(dot(pixelNorm, lightDir), 0.0);
		vec3 diffuse = diff * texColor * light.diffuse;

		vec3 specColor = texture(material.specular, texCoord).xyz;
		float spec = 0.0;
		if (blinn == 0) 
		{
			vec3 viewDir = normalize(viewPos - position);
			vec3 reflectDir = reflect(-lightDir, pixelNorm);
			spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		}
		else 
		{
			vec3 viewDir = normalize(viewPos - position);
			vec3 halfDir = normalize(lightDir + viewDir);
			spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
		}
		vec3 specular = spec * specColor * light.specular;
		result += (diffuse + specular) * intensity;
	}
	result *= attenuation;
	fragColor = vec4(result, 1.0);
}