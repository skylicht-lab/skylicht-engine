Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);

Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);

TextureCube uTexReflect : register(t3);
SamplerState uTexReflectSampler : register(s3);

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
static const float MinReflectance = 0.04;

float getPerceivedBrightness(float3 color)
{
    return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}


float solveMetallic(float3 diffuse, float3 specular, float oneMinusSpecularStrength)
{
    float specularBrightness = getPerceivedBrightness(specular);
    float diffuseBrightness = getPerceivedBrightness(diffuse);

    float a = MinReflectance;
    float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
    float c = MinReflectance - specularBrightness;
    float D = b * b - 4.0 * a * c;

    return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

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
	
	// Solver metallic
	float roughness = 1.0 - specMap.g;

	float3 f0 = specMap.r;
	float3 specularColor = f0;
    float oneMinusSpecularStrength = 1.0 - specMap.r;
	float metallic = solveMetallic(diffuseMap.rgb, specularColor, oneMinusSpecularStrength);
	
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = diffuseMap.rgb * (float3(1.0, 1.0, 1.0) - f0) * (1.0 - metallic);
    specularColor = lerp(f0, diffuseMap.rgb, metallic);
	
	// SH Ambient
	float3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	
	// Lighting
	float NdotL = max(dot(input.worldNormal, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * uLightColor.rgb;
	float3 color = directionalLight * diffuseColor;

	// Specular
	float3 H = normalize(input.worldLightDir + input.worldViewDir);	
	float NdotE = max(0.0,dot(input.worldNormal, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * specularColor;
	
	// IBL lighting (2 bounce)
	color += ambientLighting * diffuseColor * 2.0 / PI;
	
	// IBL reflection
	float3 reflection = -normalize(reflect(input.worldViewDir, input.worldNormal));
	color += uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness * 8).xyz * specularColor * metallic;	
	
	return float4(color, 1.0);
}