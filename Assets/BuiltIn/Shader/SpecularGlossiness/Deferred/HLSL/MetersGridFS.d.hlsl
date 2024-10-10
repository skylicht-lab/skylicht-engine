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
#ifdef CUSTOM_COLOR
	float4 uLightColor;
	float4 uDarkColor;
	float4 uBorderColor;
#endif
};
#endif

#ifndef CUSTOM_COLOR
static const float4 uLightColor = float4(0.3, 0.3, 0.307843, 1.0);
static const float4 uDarkColor = float4(0.156863, 0.156863, 0.160784, 1.0);
static const float4 uBorderColor = float4(0.8, 0.8, 0.8, 1.0);
#endif

// https://godotshaders.com/shader/meters-grid
// https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8

static const float gridSize = 4.0;
static float border = gridSize / 200.0;

float gridBorder(float3 uv3, float borderWidth, float aa)
{
	float2 uv = uv3.xz + uv3.xy + uv3.yz;
	
	float2 dx = ddx(uv);
	float2 dy = ddy(uv);
	
	float4 uvDDXY = float4(dx, dy);
	float2 uvDeriv = float2(length(uvDDXY.xz), length(uvDDXY.yw));
	
	float2 targetWidth = float2(borderWidth, borderWidth);
	
	float2 lineAA = uvDeriv * 1.5 * aa;
	float2 gridUV = float2(1.0, 1.0) - abs(frac(uv3.xz) * 2.0 - float2(1.0, 1.0));
	
	float2 drawWidth = clamp(targetWidth, uvDeriv, 1.0);
	float2 gridBase  = smoothstep(drawWidth  + lineAA, drawWidth - lineAA, gridUV);
	
	float2 greyScale = clamp(targetWidth / drawWidth, 0.0, 1.0);
	gridBase *= greyScale;
	
	return lerp(gridBase.x, 1.0, gridBase.y);
}

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;
	
	float3 pos = input.worldPosition.xyz;
	
	float3 uv = pos / gridSize;
	float grid = gridBorder(uv, border, 1.0);
	
	pos.x = pos.x + gridSize / 2;
	pos.z = pos.z + gridSize / 2;
	uv = pos / (gridSize * 2);
	float checkerBoard = gridBorder(uv, 0.5, 1.5);
	
	float3 col = lerp(uLightColor.rgb, uDarkColor.rgb, checkerBoard);
	col = lerp(col, uBorderColor.rgb, grid);
	
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
