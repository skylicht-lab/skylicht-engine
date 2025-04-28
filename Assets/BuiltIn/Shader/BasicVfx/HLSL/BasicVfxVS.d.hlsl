struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#ifdef UV2
	float2 tex1: TEXCOORD1;
	float3 lightmap: LIGHTMAP;
	#ifdef INSTANCING
	float4 uvScale: TEXCOORD2;
	float4 uColor: TEXCOORD3;
	float4x4 worldMatrix: TEXCOORD4;
	#endif
#else
	#ifdef INSTANCING
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4x4 worldMatrix: TEXCOORD3;
	#endif
#endif
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
#ifdef UV2
	float2 tex1 : TEXCOORD1;
#endif
	float4 uvScale: UVSCALE;
#ifdef RIM_LIGHT
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
#endif
};

cbuffer cbPerObject
{
#ifdef INSTANCING
	float4x4 uVPMatrix;
#else	
	float4x4 uMvpMatrix;
	float4 uUVScale;
#endif
	
#ifdef RIM_LIGHT
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
#endif
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

#ifdef INSTANCING
	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4 uUVScale = input.uvScale;
#endif	

#ifdef INSTANCING
	output.color = input.uColor;
#else
	output.color = input.color;
#endif

	output.tex0 = input.tex0;
	output.uvScale = uUVScale;

#ifdef UV2
	output.tex1 = input.tex1;
#endif

#if defined(INSTANCING) || defined(RIM_LIGHT)
	float4 worldPos = mul(input.pos, uWorldMatrix);
#endif
	
#ifdef RIM_LIGHT	
	output.worldViewDir = normalize((uCameraPosition - worldPos).xyz);
	
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.worldNormal = normalize(worldNormal).xyz;
#endif

#ifdef INSTANCING
	output.pos = mul(worldPos, uVPMatrix);
#else
	output.pos = mul(input.pos, uMvpMatrix);
#endif

	return output;
}