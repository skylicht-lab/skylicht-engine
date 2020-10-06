Texture2D uSourceTex : register(t0);
SamplerState uSourceTexSampler : register(s0);

Texture2D uBlurTex : register(t1);
SamplerState uBlurTexSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float2 uTexelSize;
};


float3 upsampleFilter(const float2 uv)
{
    float4 d = uTexelSize.xyxy * float4(-1, -1, +1, +1);

    float3 s1 = uBlurTex.Sample(uBlurTexSampler, uv + d.xy).rgb;
    float3 s2 = uBlurTex.Sample(uBlurTexSampler, uv + d.zy).rgb;
    float3 s3 = uBlurTex.Sample(uBlurTexSampler, uv + d.xw).rgb;
    float3 s4 = uBlurTex.Sample(uBlurTexSampler, uv + d.zw).rgb;

    return (s1 + s2 + s3 + s4) * 0.25;
}

float4 main(PS_INPUT input) : SV_TARGET
{	
	// base color
	float3 base = uSourceTex.Sample(uSourceTexSampler, input.tex0).rgb;
		
	// blur color
	float3 blur = upsampleFilter(input.tex0);
		
	float intensity = 1.3;
	float3 m = base + blur * intensity;
	
	return float4(m, 1.0);
}