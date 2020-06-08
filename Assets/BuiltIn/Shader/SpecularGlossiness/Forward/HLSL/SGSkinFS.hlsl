Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);

Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);

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

cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uSHConst[4];
};

static const float PI = 3.1415926;
static const float EnvironmentScale = 1.0;

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0).xyz;
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3 specMap = uTexSpecularMap.Sample(uTexSpecularMapSampler, input.tex0).xyz;
	
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);
	
	// SH Ambient
	float3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	
	// Lighting
	float NdotL = max(dot(input.worldNormal, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * uLightColor.rgb;
	float3 color = directionalLight * diffuseMap;

	// Specular
	float3 H = normalize(input.worldLightDir + input.worldViewDir);	
	float NdotE = max(0.0,dot(input.worldNormal, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * uLightColor.rgb;
	
	// IBL lighting
	color += ambientLighting * diffuseMap * EnvironmentScale / PI;
	
	return float4(color, 1.0);
}