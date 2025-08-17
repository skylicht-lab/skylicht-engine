Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float2 uDebug;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	
	if (uDebug.x == 1.0)
	{
		color = float4(color.r, color.r, color.r, color.a);
	}
	else if (uDebug.x == 2.0)
	{
		color = float4(color.g, color.g, color.g, color.a);
	}
	else if (uDebug.x == 3.0)
	{
		color = float4(color.b, color.b, color.b, color.a);
	}
	else if (uDebug.x == 4.0)
	{
		color = float4(color.a, color.a, color.a, 1.0);
	}
	
	if (uDebug.y == 1.0)
	{
		color.a = 1.0;
	}
	
	float4 result = input.color * color;
	return result;
}