struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 sh0: SH1;
	float3 sh1: SH2;
	float3 sh2: SH3;
	float3 sh3: SH4;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	// fast SH4
	float3 ambient = input.sh0 +
		input.sh1 * input.worldNormal.y +
		input.sh2 * input.worldNormal.z +
		input.sh3 * input.worldNormal.x;
	
	return float4(ambient * 0.9, 1.0);	// fix 0.9 for SH4
}
