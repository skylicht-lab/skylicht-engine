Texture2D uSourceTex : register(t0);
SamplerState uSourceTexSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uCurve;
};

float brightness(float3 c)
{
	return max(max(c.r, c.g), c.b);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 m = uSourceTex.Sample(uSourceTexSampler, input.tex0).rgb;
	float br = brightness(m);

	// Under-threshold part: quadratic curve
	float rq = clamp(br - uCurve.x, 0.0, uCurve.y);
	rq = uCurve.z * rq * rq;

	// Combine and apply the brightness response curve.
	m *= max(rq, br - uCurve.w) / max(br, 1e-5);

	return float4(m, 1.0);
}