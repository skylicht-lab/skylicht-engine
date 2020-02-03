struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 vPos: POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 pos = input.vPos;
	
	float depth = pos.z / pos.w;
	depth = depth * 0.5 + 0.5;
	
	return float4(depth, 0.0, 0.0, 0.0);
}