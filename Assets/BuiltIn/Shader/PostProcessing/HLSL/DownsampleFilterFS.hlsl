Texture2D uSourceTex : register(t0);
SamplerState uSourceTexSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

// adding constant buffer for transform matrices
cbuffer cbPerFrame
{
	float2 uTexelSize;
};


float brightness(float3 c)
{
    return max(max(c.r, c.g), c.b);
}

float3 downsampleAntiFlickerFilter(const float2 uv)
{
    float4 d = float4(-uTexelSize.x, -uTexelSize.y, uTexelSize.x, uTexelSize.y);

    float3 s1 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.xy, 0).rgb;
    float3 s2 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.zy, 0).rgb;
    float3 s3 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.xw, 0).rgb;
    float3 s4 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.zw, 0).rgb;

    // Karis's luma weighted average (using brightness instead of luma)
    float s1w = 1 / (brightness(s1) + 1);
    float s2w = 1 / (brightness(s2) + 1);
    float s3w = 1 / (brightness(s3) + 1);
    float s4w = 1 / (brightness(s4) + 1);
    float oneDivWSum = 1 / (s1w + s2w + s3w + s4w);

    return (s1 * s1w + s2 * s2w + s3 * s3w + s4 * s4w) * oneDivWSum;
}

float4 main(PS_INPUT input) : SV_TARGET
{	
	float3 m = downsampleAntiFlickerFilter(input.tex0);	
	return float4(m, 1.0);
}