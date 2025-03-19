Texture2D uTexture : register(t0);
SamplerState uTextureSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 vertexPos: VERTEX_POSITION;
};

cbuffer cbPerFrame
{
	float4 uColorIntensity;
#ifdef DISSOLVE
	float4 uNoiseScale;
	float4 uDissolveColor;
#endif	
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

#ifdef DISSOLVE
#include "../../Noise/HLSL/LibNoise.hlsl"
#endif

float4 main(PS_INPUT input) : SV_TARGET
{
#ifdef SOILD_COLOR
	float4 texColor = float4(input.color.rgb, 1.0f);
#else
	float4 texColor = uTexture.Sample(uTextureSampler, input.tex0);
#endif

#if defined(SOILD)
	float3 color = sRGB(texColor.rgb * input.color.rgb);
#elif defined(SOILD_COLOR)
	float3 color = sRGB(texColor.rgb);
#else	
	float3 color = sRGB(texColor.rgb * input.color.rgb * uColorIntensity.rgb);
#endif

	float alpha = texColor.a * input.color.a;

	
#ifdef DISSOLVE
	float n = pnoise(input.vertexPos * uNoiseScale.xyz);
	n = 0.5 + 0.5*n;
	
	float dissolve = n - (1.0 - alpha);
	if (dissolve <= 0.0)
		discard;
		
	color += sRGB(uDissolveColor.rgb * uColorIntensity.rgb) * step(dissolve, uNoiseScale.w) * n;
#endif
	
#ifdef LIGHTING
	// Direction lighting
	float NdotL = max(dot(input.worldNormal, uLightDirection.xyz), 0.0);
	float3 directionalLight = NdotL * sRGB(uLightColor.rgb);
	color = directionalLight * color * 0.3;
	
	// SH4 Ambient
	float3 ambientLighting = shAmbient(input.worldNormal);
	color += sRGB(ambientLighting * texColor.rgb) / PI;
#endif
	
	return float4(color, texColor.a * alpha);
}