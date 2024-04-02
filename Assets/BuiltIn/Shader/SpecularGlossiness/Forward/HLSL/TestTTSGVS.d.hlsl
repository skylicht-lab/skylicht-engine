Texture2D uTransformTexture : register(t0);
SamplerState uTransformTextureSampler : register(s0);

struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
#endif
};

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};
#else
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};
#endif

cbuffer cbPerObject
{
	float4x4 uVpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uUVScale;
	float2 uTransformTextureSize;
	float2 uTransformXY;
};

float4x4 getTransformFromTexture(float2 p)
{
	float centerX = 0.5 / uTransformTextureSize.x;
	float centerY = 0.5 / uTransformTextureSize.y;
	
	float nextPixelX = 1.0 / uTransformTextureSize.x;
	float nextPixelY = 1.0 / uTransformTextureSize.y;
	
	float2 uv = float2(
		p.x * nextPixelX * 4 + centerX, 
		p.y * nextPixelY + centerY
	);
	
	float4 c1 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0);
	uv.x = uv.x + nextPixelX;
	float4 c2 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0);
	uv.x = uv.x + nextPixelX;
	float4 c3 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0);
	uv.x = uv.x + nextPixelX;
	float4 c4 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0);
	uv.x = uv.x + nextPixelX;
	
	return float4x4(c1, c2, c3, c4);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 worldMatrix = mul(getTransformFromTexture(uTransformXY), uWorldMatrix);

	float4x4 mvpMatrix = mul(worldMatrix, uVpMatrix);
	output.pos = mul(input.pos, mvpMatrix);

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	output.tangentw = input.data.x;
#endif

	float4 worldPos = mul(input.pos, worldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);
	float4 worldNormal = mul(float4(input.norm.xyz, 0.0), worldMatrix);
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	float4 worldTangent = mul(float4(input.tangent.xyz, 0.0), worldMatrix);
#endif

	output.worldPosition = worldPos.xyz;

	output.worldNormal = normalize(worldNormal.xyz);
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

	output.worldViewDir = worldViewDir.xyz;
	output.worldLightDir = normalize(uLightDirection.xyz);
	output.viewPosition = output.pos;

	return output;
}