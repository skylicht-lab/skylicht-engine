Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);
Texture2D uTexPosition : register(t1);
SamplerState uTexPositionSampler : register(s1);
Texture2D uTexNormal : register(t2);
SamplerState uTexNormalSampler : register(s2);
Texture2D uTexData : register(t3);
SamplerState uTexDataSampler : register(s3);
Texture2DArray uShadowMap : register(t4);
SamplerState uShadowMapSampler : register(s4);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uAmbientLightColor;
	float4 uLightColor;
	float2 uShadowDistance;
	float4x4 uShadowMatrix[2];
};
float shadow(const float4 shadowCoord[2], const float shadowDistance[2], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.0002;
	float depth = 0.0;
	float sampledDistance = 0.0;
	if (farDistance > shadowDistance[0])
		id = 1;
	if (farDistance > shadowDistance[1])
		return 1.0;
	depth = shadowCoord[id].z;
	sampledDistance = uShadowMap.Sample(uShadowMapSampler, float3(shadowCoord[id].xy, id)).r;
	if (depth - bias > sampledDistance)
        visible = 0.1f;
	return visible;
}
static const float MinReflectance = 0.04;
float getPerceivedBrightness(float3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}
float solveMetallic(float3 diffuse, float3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}
float3 SG(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 ambient,
	const float3 lightColor,
	const float visibility)
{
	float roughness = 1.0 - gloss;
	float3 f0 = spec;
	float3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = baseColor.rgb * (float3(1.0, 1.0, 1.0) - f0) * (1.0 - metallic);
	specularColor = lerp(f0, baseColor.rgb, metallic);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	float3 color = (ambient + NdotL * lightColor * visibility) * (diffuseColor + specular * specularColor * visibility);
	return color;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);
	float depth = length(v);
	float4 shadowCoord[2];
	shadowCoord[0] = mul(float4(position, 1.0), uShadowMatrix[0]);
	shadowCoord[1] = mul(float4(position, 1.0), uShadowMatrix[1]);
	float shadowDistance[2];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	float3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uAmbientLightColor.rgb,
		uLightColor.rgb,
		visibility);
	return float4(color, 1.0);
}
