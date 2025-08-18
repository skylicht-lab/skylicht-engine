Texture2D uTexDiffuse1 : register(t0);
SamplerState uTex1Sampler : register(s0);

#if defined(UV2) || defined(LAYER2)
Texture2D uTexDiffuse2 : register(t1);
SamplerState uTex2Sampler : register(s1);
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
#ifdef UV2
	float2 tex1 : TEXCOORD1;
#endif
	float4 uvScale: UVSCALE0;
#ifdef UV2
	float4 uvScale1: UVSCALE1;
#endif	
#ifdef RIM_LIGHT
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
#endif
};

cbuffer cbPerFrame
{
	float4 uColor1;
#if defined(UV2) || defined(LAYER2)	
	float4 uColor2;
#endif
	float4 uColorIntensity;
	float4 uTime;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float2 tex0 = input.tex0 * input.uvScale.xy + input.uvScale.zw;

#if defined(UV2)
	float2 tex1 = input.tex1 * input.uvScale1.xy + input.uvScale1.zw;
#endif

#ifdef SCROLL_Y
	float2 uvOffset = float2(0.0, uTime.y) / 4.0;
#else
	float2 uvOffset = float2(-uTime.y, 0.0) / 4.0;
#endif
	
#if defined(SCROLL_UV1)
	float4 color1 = uTexDiffuse1.Sample(uTex1Sampler, tex0 + uvOffset);
#elif defined(NO_TILE_UV1)
	float4 color1 = uTexDiffuse1.Sample(uTex1Sampler, input.tex0);
#else
	float4 color1 = uTexDiffuse1.Sample(uTex1Sampler, tex0);
#endif

#if defined(UV2)
// use 2 uv
#ifdef SCROLL_UV2
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, tex1 + uvOffset);
#else
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, tex1);
#endif
#elif defined(LAYER2)
// use 1 uv
#ifdef SCROLL_LAYER2
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, tex0 + uvOffset);
#else
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, tex0);
#endif
#endif

#ifdef RIM_LIGHT
	float VdotN = max(dot(input.worldViewDir, input.worldNormal), 0.0);
	float rimLightIntensity = max(0.0, 1.0 - VdotN);
	rimLightIntensity = pow(rimLightIntensity, 0.9);
#endif

#if defined(UV2) || defined(LAYER2)
	float4 result = (color1 * uColor1 + color2 * uColor2) * input.color * uColorIntensity;
	result.a = color1.a * color2.a * uColor1.a * uColor2.a;
#ifdef RIM_LIGHT
	result.a = result.a * rimLightIntensity;
#endif	
	return result;
#else
	return color1 * input.color * uColor1 * uColorIntensity;
#endif
}