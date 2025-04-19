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
#ifdef SCROLL_Y
	float2 uvOffset = float2(0.0, uTime.y) / 4.0;
#else
	float2 uvOffset = float2(uTime.x, 0.0) / 4.0;
#endif
	
#if defined(SCROLL_UV1)
	float4 color1 = uTexDiffuse1.Sample(uTex1Sampler, input.tex0 + uvOffset);
#else
	float4 color1 = uTexDiffuse1.Sample(uTex1Sampler, input.tex0);
#endif

#if defined(UV2)
// use 2 uv
#ifdef SCROLL_UV2
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, input.tex1 + uvOffset);
#else
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, input.tex1);
#endif
#elif defined(LAYER2)
// use 1 uv
#ifdef SCROLL_LAYER2
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, input.tex0 + uvOffset);
#else
	float4 color2 = uTexDiffuse2.Sample(uTex2Sampler, input.tex0);
#endif
#endif

#if defined(UV2) || defined(LAYER2)
	float4 result = (color1 * uColor1 + color2 * uColor2) * input.color * uColorIntensity;
	result.a = color1.a * color2.a * uColor1.a * uColor2.a;
	return result;
#else
	return color1 * input.color * uColor1 * uColorIntensity;
#endif
}