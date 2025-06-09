#ifndef NO_TEXTURE
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

#ifndef NO_NORMAL_MAP
Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);
#endif

#ifndef NO_SPECGLOSS
Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);
#endif
#endif

TextureCube uTexReflect : register(t3);
SamplerState uTexReflectSampler : register(s3);

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
};
#else
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
};
#endif

cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
#if defined(NO_TEXTURE) || defined(NO_SPECGLOSS)
	float2 uSpecGloss;
#endif
	float2 uLightMul;
	float4 uSHConst[4];
};

#include "../../../PostProcessing/HLSL/LibToneMapping.hlsl"
#include "../../../SHAmbient/HLSL/SHAmbient.hlsl"

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

float4 main(PS_INPUT input) : SV_TARGET
{
#ifdef NO_TEXTURE
	float4 diffuseMap = uColor;
	float3 specMap = float3(uSpecGloss, 0.0);
#else
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;

	#ifdef NO_SPECGLOSS
	float3 specMap = float3(uSpecGloss, 0.0);
	#else
	float3 specMap = uTexSpecularMap.Sample(uTexSpecularMapSampler, input.tex0).xyz;
	#endif
#endif

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
	float3 n = input.worldNormal;
#else
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
#endif

	// Solver metallic
	float spec = specMap.r;
	float gloss = specMap.g;
	float roughness = 1.0 - gloss;

	float3 diffuseColor = diffuseMap.rgb;

	float3 f0 = spec;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(diffuseColor, f0, oneMinusSpecularStrength);

	f0 = float3(0.1, 0.1, 0.1);	
	float3 specularColor = lerp(f0, diffuseColor, metallic);

	// SH4 Ambient
	float3 ambientLighting = shAmbient(n);

	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	diffuseColor = sRGB(diffuseColor);
	specularColor = sRGB(specularColor);
	float3 lightColor = sRGB(uLightColor.rgb);

	float c = (1.0 - spec * gloss);

	// Lighting
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * lightColor;
	float3 color = (directionalLight * diffuseColor) * (0.1 + roughness * 0.3) * c * uLightMul.y;

	// Specular
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;

	// IBL lighting
	color += ambientLighting * diffuseColor * (0.1 + c * 0.9) / PI;
	
	// IBL reflection
	float3 reflection = -normalize(reflect(input.worldViewDir, n));
	
	float brightness = (0.8 + gloss * 1.8);
	color += sRGB(uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness * 7).xyz) * brightness * specularColor;
	
	return float4(color, diffuseMap.a);
}