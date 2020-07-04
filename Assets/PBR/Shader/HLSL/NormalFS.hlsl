Texture2D uTexNormalMap : register(t0);
SamplerState uTexNormalMapSampler : register(s0);

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

float4 main(PS_INPUT input) : SV_TARGET
{	
	float3 diffuseMap = float3(0.7, 0.7, 0.7);
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	
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
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * uLightColor.rgb * diffuseMap;
	float3 color = directionalLight;

	// Specular
	float3 H = normalize(input.worldLightDir + input.worldViewDir);	
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 100.0f * 0.5) * 0.5;
	color += float3(specular, specular, specular);
	
	// IBL lighting
	color += ambientLighting * diffuseMap * 1.5 / PI;
	
	return float4(color, 1.0);
}