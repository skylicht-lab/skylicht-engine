precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform sampler2D uTexLight;
uniform sampler2DArray uShadowMap;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec3 uShadowDistance;
uniform mat4 uShadowMatrix[3];
in vec2 varTexCoord0;
out vec4 FragColor;
float texture2DCompare(vec3 uv, float compare){
	float depth = texture(uShadowMap, uv).r;
	return step(compare, depth);
}
float shadow(const vec4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.001;
	float depth = 0.0;
	float result = 0.0;
	float size = 2048.0;
	if (farDistance > shadowDistance[0])
		id = 1;
	if (farDistance > shadowDistance[1])
		id = 2;
	if (farDistance > shadowDistance[2])
		return 1.0;
	depth = shadowCoord[id].z;
	vec2 uv = shadowCoord[id].xy;
	for(int x=-1; x<=1; x++)
	{
		for(int y=-1; y<=1; y++)
		{
			vec2 off = vec2(x,y)/size;
			result += texture2DCompare(vec3(uv+off, id), depth - bias);
		}
	}
	return result/9.0;
}
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
	const vec3 lightColor,
	const float visibility,
	const vec4 light)
{
	float roughness = 1.0 - gloss;
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb;
	specularColor = mix(f0, baseColor.rgb, metallic);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	vec3 directionalLight = NdotL * lightColor * visibility;
	vec3 color = (directionalLight + light.rgb) * diffuseColor + (specular * specularColor * visibility + light.a * specularColor);
	return color;
}
void main(void)
{
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec4 light = texture(uTexLight, varTexCoord0.xy);
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);
	float depth = length(v);
	vec4 shadowCoord[3];
	shadowCoord[0] = uShadowMatrix[0] * vec4(position, 1.0);
	shadowCoord[1] = uShadowMatrix[1] * vec4(position, 1.0);
	shadowCoord[2] = uShadowMatrix[2] * vec4(position, 1.0);
	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	vec3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb,
		visibility,
		light);
	FragColor = vec4(color, 1.0);
}
