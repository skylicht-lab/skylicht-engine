Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexRamp : register(t1);
SamplerState uTexRampSampler : register(s1);

#if defined(SHADOW)
Texture2DArray uShadowMap : register(t2);
SamplerState uShadowMapSampler : register(s2);
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldPos: WORLDPOSITION;
	float3 depth: DEPTH;
	float4 color: COLOR;
};

cbuffer cbPerFrame
{
	float4 uLightDirection;
	float4 uLightColor;
	float4 uColor;
	float4 uShadowColor;
	float2 uWrapFactor;
	float3 uSpecular;
	float4 uSHConst[4];
#if defined(SHADOW)
	float3 uShadowDistance;
	float4x4 uShadowMatrix[3];
#endif
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"
#include "../../SHAmbient/HLSL/SHAmbient.hlsl"

#if defined(SHADOW)
#include "../../Shadow/HLSL/LibShadow.hlsl"
#endif

static const float PI = 3.1415926;

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuse = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	float3 diffuseMap = sRGB(diffuse.rgb);
	
#ifdef INSTANCING
	float3 color = sRGB(input.color.rgb);
	float shadowIntensity = input.color.a;
#else
	float3 color = sRGB(uColor.rgb * input.color.rgb);
	float shadowIntensity = uColor.a;
#endif

	float3 shadowColor = sRGB(uShadowColor.rgb);
	float3 lightColor = sRGB(uLightColor.rgb);
	
	float visibility = 1.0;
	
#if defined(SHADOW)
	// shadow
	float depth = length(input.depth);

#if defined(CASCADED_SHADOW)
	float4 shadowCoord[3];
	shadowCoord[0] = mul(float4(input.worldPos, 1.0), uShadowMatrix[0]);
	shadowCoord[1] = mul(float4(input.worldPos, 1.0), uShadowMatrix[1]);
	shadowCoord[2] = mul(float4(input.worldPos, 1.0), uShadowMatrix[2]);

	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	visibility = shadow(shadowCoord, shadowDistance, depth);
#else
	float4 shadowCoord = mul(float4(input.worldPos, 1.0), uShadowMatrix[0]);
	visibility = shadow(shadowCoord, depth);
#endif	// CASCADED_SHADOW

#endif // SHADOW

	// SH4 Ambient
	float3 ambientLighting = shAmbient(input.worldNormal);
	
	// Tone Mapping
	ambientLighting = sRGB(ambientLighting * 0.9);
	
	float NdotL = max((dot(input.worldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x), 0.0);
	float3 rampMap = uTexRamp.Sample(uTexRampSampler, float2(NdotL, NdotL)).rgb;

	// Shadows intensity through alpha
	float3 ramp = lerp(color, shadowColor, shadowIntensity * (1.0 - visibility));
	ramp = lerp(ramp, color, rampMap);
	
	// Specular
	float3 h = normalize(uLightDirection.xyz + input.worldViewDir);
	float NdotH = max(0, dot(input.worldNormal, h));
	
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	
	return float4(diffuseMap * lightColor * ramp * (visibility * (1.0 - uWrapFactor.y)) + lightColor * spec * visibility + uWrapFactor.y * diffuseMap * ambientLighting / PI, diffuse.a);
}