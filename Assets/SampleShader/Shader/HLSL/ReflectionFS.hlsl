Texture2D uTexNormalMap : register(t0);
SamplerState uTexNormalMapSampler : register(s0);

TextureCube uTexReflect : register(t1);
SamplerState uTexReflectSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color: COLOR;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
};

static const float PI = 3.1415926;

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 diffuseMap = float3(0.5, 0.5, 0.5);
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;

	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);

	float3 color = float3(0.0, 0.0, 0.0);

	// Specular
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 100.0f * 0.5) * 0.5;
	color += float3(specular, specular, specular);

	// Reflection
	float3 reflection = -normalize(reflect(input.worldViewDir, n));
	color += uTexReflect.SampleLevel(uTexReflectSampler, reflection, 0).xyz;

	return float4(color, 1.0);
}