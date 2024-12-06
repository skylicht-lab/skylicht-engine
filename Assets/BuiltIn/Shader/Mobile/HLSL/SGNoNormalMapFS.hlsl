// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.hlsl : NO_NORMAL_MAP]
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);
Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
};
cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
	float4 uSHConst[4];
};
static const float gamma = 2.2;
static const float invGamma = 1.0 / 2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
float3 shAmbient(float3 n)
{
	float3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	return ambientLighting * 0.9;
}
static const float PI = 3.1415926;
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;
	float3 specMap = uTexSpecularMap.Sample(uTexSpecularMapSampler, input.tex0).xyz;
	float3 n = input.worldNormal;
	float3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	float3 diffuseColor = sRGB(diffuseMap.rgb);
	float3 lightColor = sRGB(uLightColor.rgb);
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * lightColor;
	float3 color = directionalLight * diffuseColor;
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * diffuseColor;
	color += ambientLighting * diffuseColor / PI;
	return float4(color, diffuseMap.a);
}