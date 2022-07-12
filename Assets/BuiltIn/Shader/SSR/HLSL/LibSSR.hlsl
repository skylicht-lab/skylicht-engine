// References
// https://github.com/MonkeyFirst/Urho3D_SSR_Shader/blob/master/bin/CoreData/Shaders/GLSL/SSR.glsl

#define BINARY_SEARCH() {\
	projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);\
	projectedCoord.xy = projectedCoord.xy / projectedCoord.w;\
	projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;\
	testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy);\
	dDepth = rayPosition.z - testPosition.w;\
	dir *= 0.5;\
	if(dDepth > 0.0) rayPosition -= dir;\
	else rayPosition += dir;\
}

#define SSR_RAY_MARCH() {\
	rayPosition += dir;\
	projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);\
	projectedCoord.xy = projectedCoord.xy / projectedCoord.w;\
	ssrUV = uvScale * projectedCoord.xy + uvOffset;\
	testPosition = uTexPosition.Sample(uTexPositionSampler, ssrUV);\
	if(rayPosition.z - testPosition.w >= 0.0)\
	{\
		ssrUV = binarySearch(dir, rayPosition);\
		return ssrUV;\
	}\
}

float2 binarySearch(float3 dir, float3 rayPosition)
{
	float4 projectedCoord;
	float4 testPosition;
	float dDepth;
	
	const float2 uvOffset = float2(0.5, 0.5);
	const float2 uvScale = float2(0.5, -0.5);
	
	// 16 Step
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	BINARY_SEARCH();
	
	return projectedCoord.xy;
}

float2 ssrRayMarch(const float4 position, const float3 reflection)
{
	float4 projectedCoord;
	
	// convert to view space
	float3 rayPosition = mul(float4(position.xyz, 1.0), uView).xyz;
	float3 viewReflection = normalize(mul(float4(reflection, 0.0), uView).xyz);
	
	// step 0.5m
	float3 dir = viewReflection * 0.5;

	float2 ssrUV;
	float4 testPosition;
	
	const float2 uvOffset = float2(0.5, 0.5);
	const float2 uvScale = float2(0.5, -0.5);
	
	// rayMarch test 32 step
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	SSR_RAY_MARCH();
	
	return ssrUV;
}

float3 SSR(const float3 baseColor, const float4 position, const float3 reflection, const float roughness)
{
	float2 ssrUV = ssrRayMarch(position, reflection);
	
	// z clip when camera look down
	float z = mul(float4(reflection, 0.0), uView).z;
	z = clamp(z, 0.0, 1.0);
	
	// convert 3d position to 2d texture coord
	float mipLevel = roughness * 5.0;
	float3 color = uTexLastFrame.SampleLevel(uTexLastFrameSampler, ssrUV, mipLevel).rgb;
	
	// edge factor
	float2 dCoords = smoothstep(float2(0.0, 0.0), float2(0.5, 0.5), abs(float2(0.5, 0.5) - ssrUV));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	return lerp(baseColor * 0.8, color, screenEdgefactor * z);
	
	// test
	// return float3(screenEdgefactor * z, screenEdgefactor * z, screenEdgefactor * z);
}