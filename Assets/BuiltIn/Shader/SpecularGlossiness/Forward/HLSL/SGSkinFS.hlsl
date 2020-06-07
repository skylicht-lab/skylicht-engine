Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);

struct PS_INPUT
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
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0).xyz;
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);
	
	return float4(n, 1.0);
}