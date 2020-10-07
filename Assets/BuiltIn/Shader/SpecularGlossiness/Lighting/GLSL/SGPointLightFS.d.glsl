precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;

uniform vec4 uCameraPosition;
uniform vec4 uLightPosition;
uniform vec4 uLightAttenuation;
uniform vec4 uLightColor;

in vec2 varTexCoord0;

out vec4 FragColor;

void main(void)
{
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;

	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);

	float spec = data.r;
	float gloss = data.g;

	// Lighting	
	vec3 direction = uLightPosition.xyz - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y)) * uLightColor.a;

	vec3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));

	// Specular
	vec3 H = normalize(direction + viewDir);
	float NdotE = max(0.0, dot(normal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;

	vec3 lightColor = uLightColor.rgb * (NdotL * attenuation);

	FragColor = vec4(lightColor, specular * attenuation);
}