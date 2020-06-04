TextureCube uTexReflect : register(t0);
SamplerState uTexReflectSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 worldNormal: WORLDNORMAL;
};

float4 main(PS_INPUT input) : SV_TARGET
{	
	float4 result = input.color * uTexReflect.SampleLevel(uTexReflectSampler, input.worldNormal, 0);
	return result;
}