struct PS_INPUT
{
	float4 pos : SV_POSITION;
};

cbuffer cbPerFrame
{
	float4 uAmbientColor;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return uAmbientColor;
}