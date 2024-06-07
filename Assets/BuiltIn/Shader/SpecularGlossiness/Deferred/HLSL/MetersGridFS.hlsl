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
	float2 uSpecGloss;
};
#endif

static const float3 lightColor = float3(0.3, 0.3, 0.307843);
static const float3 darkColor = float3(0.156863, 0.156863, 0.160784);
static const float3 borderColor = float3(0.8, 0.8, 0.8);

static const float gridSize = 4.0;
static float h = gridSize / 2.0;
static float border = gridSize / 1000.0;

#define mod(x,y) (x-y*floor(x/y))

bool borde(float pos) 
{
	float m1_0 = mod(pos, 1.0);
	float m0_5 = mod(pos, 0.5);
	
	return 
		m1_0 < border || 
		(1.0 - m1_0) < border || 
		m0_5 < border || 
		(0.5 - m0_5) < border;
}

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;
	
	float3 pos = input.worldPosition.xyz;
	
	// This is to avoid the zero incoherence
	if (pos.x <= 0.0) pos.x = abs(pos.x - h);
	if (pos.y <= 0.0) pos.y = abs(pos.y - h);
	if (pos.z <= 0.0) pos.z = abs(pos.z - h);
	pos /= gridSize;
	
	pos.y += float(frac(float(int(pos.x*h))/h));
	pos.z += float(frac(float(int(pos.y*h))/h));
	
	// grid color
	float3 col = float3(0.0, 0.0, 0.0);
	
	if (frac(float(int(pos.z*h))/h) == 0.0) 
		col += lightColor;
	else 
		col += darkColor;
	
	// border color
	if (borde(pos.x)) col = borderColor;
	// if (borde(pos.y)) col = borderColor;
	if (borde(pos.z)) col = borderColor;
	
	
#ifdef INSTANCING
	output.Diffuse = input.color * float4(col.r, col.g, col.b, 1.0);
	output.SG = float4(input.specGloss.x, input.specGloss.y, 0.0, 1.0);
#else
	output.Diffuse = uColor * float4(col.r, col.g, col.b, 1.0);
	output.SG = float4(uSpecGloss.x, uSpecGloss.y, 0.0, 1.0);
#endif	
	
	output.Position = input.worldPosition;
	output.Normal = float4(input.worldNormal, 1.0);

	return output;
}
