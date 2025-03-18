Texture2D uTexture : register(t0);
SamplerState uTextureSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
};

cbuffer cbPerFrame
{
	float4 uColorIntensity;
#ifdef LIGHTING
	float4 uLightColor;
	float4 uLightDirection;	
	float4 uSHConst[4];
#endif
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"
#ifdef LIGHTING
#include "../../SHAmbient/HLSL/SHAmbient.hlsl"
static const float PI = 3.1415926;
#endif

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 texColor = uTexture.Sample(uTextureSampler, input.tex0);
	float3 color = sRGB(texColor.rgb * input.color.rgb * uColorIntensity.rgb);
	
#ifdef LIGHTING
	// Direction lighting
	float NdotL = max(dot(input.worldNormal, uLightDirection.xyz), 0.0);
	float3 directionalLight = NdotL * sRGB(uLightColor.rgb);
	color = directionalLight * color * 0.3;
	
	// SH4 Ambient
	float3 ambientLighting = shAmbient(input.worldNormal);
	color += sRGB(ambientLighting) * texColor.rgb / PI;
#endif
	
	return float4(color, texColor.a * input.color.a);
}