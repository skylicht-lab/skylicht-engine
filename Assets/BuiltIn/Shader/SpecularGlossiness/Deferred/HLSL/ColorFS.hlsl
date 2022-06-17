struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
#ifdef INSTANCING
	float4 color: COLOR;
	float2 specGloss: SPECGLOSS;
#endif
};

struct PS_OUTPUT
{
	float4 Diffuse: SV_TARGET0;
	float4 Position: SV_TARGET1;
	float4 Normal: SV_TARGET2;
	float4 SG: SV_TARGET3;
};

#ifndef INSTANCING
cbuffer cbPerFrame
{
	float4 uColor;
	float uSpec;
	float uGloss;
};
#endif

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;

#ifdef INSTANCING
	output.Diffuse = input.color;
	output.SG = float4(input.specGloss.x, input.specGloss.y, 0.0, 1.0);
#else
	output.Diffuse = uColor;
	output.SG = float4(uSpec, uGloss, 0.0, 1.0);
#endif
	
	output.Position = input.worldPosition;
	output.Normal = float4(input.worldNormal, 1.0);

	return output;
}
