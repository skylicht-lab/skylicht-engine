precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uAmbientLightColor;
uniform vec4 uLightColor;
in vec2 varTexCoord0;
out vec4 FragColor;
const float EnvironmentScale = 3.0;
const float PI = 3.1415926;
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
vec3 SG(
	const vec3 baseColor,
	const float spec,
	const float gloss,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 ambient,
	const vec3 lightColor)
{
	float roughness = 1.0 - gloss;
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb * (vec3(1.0, 1.0, 1.0) - f0) * (1.0 - metallic);
	specularColor = mix(f0, baseColor.rgb, metallic);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	vec3 color = (ambient + NdotL * lightColor) * (diffuseColor + specular * specularColor);
	return color;
}
void main(void)
{
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);
	vec3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uAmbientLightColor.rgb,
		uLightColor.rgb);
	FragColor = vec4(color, 1.0);
}
