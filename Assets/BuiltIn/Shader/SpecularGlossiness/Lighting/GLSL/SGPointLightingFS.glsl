precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform vec4 uCameraPosition;
uniform vec4 uLightPosition;
uniform vec4 uLightAttenuation;
uniform vec4 uLightColor;
in vec2 varTexCoord0;
out vec4 FragColor;
const float MinReflectance = 0.04;
float getPerceivedBrightness(vec3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}
float solveMetallic(vec3 diffuse, vec3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}
void main(void)
{
	vec3 baseColor = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);
	float spec = data.r;
	float gloss = data.g;
	float roughness = 1.0 - gloss;
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb;
	specularColor = mix(f0, baseColor.rgb, metallic);
	vec3 direction = uLightPosition.xyz - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y));
	vec3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));
	vec3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	vec3 lightColor = uLightColor.rgb * (NdotL * attenuation) + specular * specularColor.rgb * attenuation;
	FragColor = vec4(lightColor, 1.0);
}
