Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);
Texture2D uTexPosition : register(t1);
SamplerState uTexPositionSampler : register(s1);
Texture2D uTexNormal : register(t2);
SamplerState uTexNormalSampler : register(s2);
Texture2D uTexData : register(t3);
SamplerState uTexDataSampler : register(s3);
Texture2D uTexLight : register(t4);
SamplerState uTexLightSampler : register(s4);
Texture2D uTexIndirect : register(t5);
SamplerState uTexIndirectSampler : register(s5);
Texture2DArray uShadowMap : register(t6);
SamplerState uShadowMapSampler : register(s6);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uLightColor;
	float2 uLightMultiplier;
	float3 uShadowDistance;
	float4x4 uShadowMatrix[3];
};
float texture2DCompare(float3 uv, float compare){
	float depth = uShadowMap.SampleLevel(uShadowMapSampler, uv, 0).r;
	return step(compare, depth);
}
float shadow(const float4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.0001;
	float depth = 0.0;
	float result = 0.0;
	float size = 2048;
	if (farDistance < shadowDistance[0])
		id = 0;
	else if (farDistance < shadowDistance[1])
		id = 1;
	else if (farDistance < shadowDistance[2])
		id = 2;
	else
		return 1.0;
	float3 shadowUV = shadowCoord[id].xyz / shadowCoord[id].w;
	depth = shadowUV.z;
	float2 uv = shadowUV.xy;
	[unroll]
	for(int x=-1; x<=1; x++)
	{
		[unroll]
		for(int y=-1; y<=1; y++)
		{
			float2 off = float2(x,y)/size;
			result += texture2DCompare(float3(uv+off, id), depth - bias);
		}
	}
	return result/9.0;
}
static const float PI = 3.1415926;
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
static const float gamma = 2.2;
static const float invGamma = 1.0/2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
float3 SG(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float visibility,
	const float4 light,
	const float3 indirect,
	const float directMultiplier,
	const float indirectMultiplier)
{
	float roughness = 1.0 - gloss;
	float3 f0 = spec;
	float3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = baseColor.rgb;
	specularColor = lerp(f0, baseColor.rgb, metallic);
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	float3 directionLightColor = sRGB(lightColor);
	float3 pointLightColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	float3 directionalLight = NdotL * directionLightColor * visibility;
	float3 color = (directionalLight + pointLightColor) * diffuseColor * directMultiplier + specular * specularColor * visibility + light.a * specularColor;
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	return color;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float4 light = uTexLight.Sample(uTexLightSampler, input.tex0);
	float3 indirect = uTexIndirect.Sample(uTexIndirectSampler, input.tex0).rgb;
	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);
	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	if (dot(viewDir, normal) < 0)
	{
		normal = normal * -1.0;
		directMul = 0.3;
		indirectMul = 0.3;
	}
	float depth = length(v);
	float4 shadowCoord[3];
	shadowCoord[0] = mul(float4(position, 1.0), uShadowMatrix[0]);
	shadowCoord[1] = mul(float4(position, 1.0), uShadowMatrix[1]);
	shadowCoord[2] = mul(float4(position, 1.0), uShadowMatrix[2]);
	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	float3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb,
		visibility,
		light,
		indirect,
		directMul,
		indirectMul);
	return float4(color, 1.0);
}
