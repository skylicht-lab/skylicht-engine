struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uAmbientColor;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return uAmbientColor;
}