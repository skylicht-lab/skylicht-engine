float texture2DCompare(float2 uv, int id, float compare){
	float depth = uShadowMap.Sample(uShadowMapSampler, float3(uv.x, uv.y, id)).r;
	return step(compare, depth);	
}

float shadow(const float4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{	
	int id = 0;	
	float bias = 0.0001;
	float depth = 0.0;
	
	float result = 0.0;
	float size = 2048;
	
	if (farDistance < shadowDistance[0])
	{
		id = 0;
	}
	else if (farDistance < shadowDistance[1])
	{
		id = 1;		
	}
	else if (farDistance < shadowDistance[2])
	{
		id = 2;
	}
	else
	{
		return 1.0;
	}
	
	depth = shadowCoord[id].z;
	float2 uv = shadowCoord[id].xy;
	
	float shadowRadius = 1.0;
	float2 texelSize = float2(1.0, 1.0) / size;
	float dx0 = - texelSize.x * shadowRadius;
	float dy0 = - texelSize.y * shadowRadius;
	float dx1 = + texelSize.x * shadowRadius;
	float dy1 = + texelSize.y * shadowRadius;
	float dcompare = depth - bias;
	
	result = (
		texture2DCompare(uv + float2( dx0, dy0 ), id, dcompare) +
		texture2DCompare(uv + float2( 0.0, dy0 ), id, dcompare) +
		texture2DCompare(uv + float2( dx1, dy0 ), id, dcompare) +
		texture2DCompare(uv + float2( dx0, 0.0 ), id, dcompare) +
		texture2DCompare(uv, id, dcompare) +
		texture2DCompare(uv + float2( dx1, 0.0 ), id, dcompare) +
		texture2DCompare(uv + float2( dx0, dy1 ), id, dcompare) +
		texture2DCompare(uv + float2( 0.0, dy1 ), id, dcompare) +
		texture2DCompare(uv + float2( dx1, dy1 ), id, dcompare) +

		texture2DCompare(uv + float2( dx0 * 0.5, dy0 * 0.5), id, dcompare) +
		texture2DCompare(uv + float2( 0.0 * 0.5, dy0 * 0.5 ), id, dcompare) +
		texture2DCompare(uv + float2( dx1 * 0.5, dy0 * 0.5 ), id, dcompare) +
		texture2DCompare(uv + float2( dx0 * 0.5, 0.0 ), id, dcompare) +
		texture2DCompare(uv + float2( dx1 * 0.5, 0.0 ), id, dcompare) +
		texture2DCompare(uv + float2( dx0 * 0.5, dy1 * 0.5 ), id, dcompare) +
		texture2DCompare(uv + float2( 0.0, dy1 * 0.5 ), id, dcompare) +
		texture2DCompare(uv + float2( dx1 * 0.5, dy1 * 0.5 ), id, dcompare)
	) * ( 1.0 / 17.0 );
		
	result *= 1.5;
	return result;
}