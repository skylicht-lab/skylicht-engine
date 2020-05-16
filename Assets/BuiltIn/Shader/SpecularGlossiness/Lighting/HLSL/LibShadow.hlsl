float texture2DCompare(float3 uv, float compare){
	float depth = uShadowMap.Sample(uShadowMapSampler, uv).r;
	return step(compare, depth);
}

float shadow(const float4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{	
	int id = 0;
	float visible = 1.0;
	float bias = 0.001;
	float depth = 0.0;
	
	float result = 0.0;
	float size = 2048;
	
	if (farDistance < shadowDistance[0])
		id = 0;
	else if (farDistance < shadowDistance[1])	
		id = 1;		
	else if (farDistance < shadowDistance[2])
		id = 2;
	else
		return 1.0;
	
	depth = shadowCoord[id].z;
	float2 uv = shadowCoord[id].xy;
	
	[unroll]
	for(int x=-1; x<=1; x++)
	{
		[unroll]
		for(int y=-1; y<=1; y++)
		{
			float2 off = float2(x,y)/size;
			result += texture2DCompare(float3(uv+off, id), depth - bias);
		}
	}
	
	return result/9.0;
}