Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);

Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);

Texture2D uTexRoughMetal : register(t2);
SamplerState uTexRoughMetalSampler : register(s2);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
};

struct PS_OUTPUT
{
	float4 Diffuse: SV_TARGET0;
	float4 Position: SV_TARGET1;
	float4 Normal: SV_TARGET2;
	float4 SG: SV_TARGET3;
};

cbuffer cbPerFrame
{
	float4 uColor;
};

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 baseMap = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb * uColor.rgb;
	float3 normalMap = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 rmMap = uTexRoughMetal.Sample(uTexRoughMetalSampler, input.tex0).xyz;

	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);

	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = mul(localCoords, rotation);
	n = normalize(n);

	// convert (base rough metal) to (diff spec gloss)
	// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/a94655275e5e4e8ae580b1d95ce678b74ab87426/shaders/pbr-frag.glsl
	// line 220
	float roughness = rmMap.r;
	float metallic = rmMap.g;
	float3 f0 = float3(0.04, 0.04, 0.04);
    float3 diffuseColor = baseMap.rgb * (float3(1.0, 1.0, 1.0) - f0);
    diffuseColor *= (1.0 - metallic);
    float3 specularColor = lerp(f0, baseMap.rgb, metallic);

	output.Diffuse = float4(diffuseColor, 1.0);
	output.Position = input.worldPosition;
	output.Normal = float4(n, 1.0);
	output.SG = float4(specularColor.g, 1.0 - roughness, 1.0, 1.0);

	return output;
}
