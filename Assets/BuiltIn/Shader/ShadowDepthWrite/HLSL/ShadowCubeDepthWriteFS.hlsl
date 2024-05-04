struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 vPos: POSITION;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerFrame
{
	float4 uLightPosition;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 lightToVertex = input.worldPos.xyz - uLightPosition.xyz;

	float lightToPixelDistance = length(lightToVertex);

	return lightToPixelDistance;
}