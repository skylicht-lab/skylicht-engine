#define RAY_LENGTH 32.0

float3 SSR(const float3 baseColor, const float3 position, const float3 reflection, const float roughness)
{	
	float4 projectedCoord;
	
	float3 beginPosition;
	float3 endPosition;
	
	float3 rayPosition = position;
	float4 viewPosition;
		
	float3 dir = reflection * RAY_LENGTH;
	
	float mipLevel = roughness * 5.0;

	// ray test
	for (int i = 32; i > 0; --i)
	{
		// begin ray
		beginPosition = rayPosition;
		
		// end ray
		endPosition = rayPosition + dir;
		
		// mid ray
		rayPosition += dir * 0.5;
		
		// convert 3d position to 2d texture coord		
		projectedCoord = mul(float4(rayPosition.xyz, 1.0), uViewProjection);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		projectedCoord.xy = float2(0.5, -0.5) * projectedCoord.xy + float2(0.5, 0.5);
		
		float3 testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy).xyz;
		
		float3 d1 = testPosition - beginPosition;
		float lengthSQ1 = d1.x*d1.x + d1.y*d1.y + d1.z*d1.z;
		
		float3 d2 = testPosition - endPosition;
		float lengthSQ2 = d2.x*d2.x + d2.y*d2.y + d2.z*d2.z;
		
		// beginPosition is nearer
		if (lengthSQ1 < lengthSQ2)
		{
			rayPosition = beginPosition;
		}
		
		// binary search test
		dir *= 0.5;
	}
	
	// z clip when camera look down
	float z = mul(float4(reflection, 0.0), uView).z;
	z = clamp(z, 0.0, 1.0);
	
	// convert 3d position to 2d texture coord	
	float3 color = uTexLastFrame.SampleLevel(uTexLastFrameSampler, projectedCoord.xy, mipLevel).rgb;
	
	// edge factor
	float2 dCoords = smoothstep(float2(0.0, 0.0), float2(0.5, 0.5), abs(float2(0.5, 0.5) - projectedCoord.xy));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	return lerp(baseColor * 0.8, color, screenEdgefactor * z);
	// return float3(screenEdgefactor * z, screenEdgefactor * z, screenEdgefactor * z);
}