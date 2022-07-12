// References
// https://github.com/MonkeyFirst/Urho3D_SSR_Shader/blob/master/bin/CoreData/Shaders/GLSL/SSR.glsl
vec2 binarySearch(vec3 dir, vec3 rayPosition)
{
	vec4 projectedCoord;
	
	const vec2 uvOffset = vec2(0.5, 0.5);
	const vec2 uvScale = vec2(0.5, 0.5);
	
	vec4 testPosition;
	float dDepth;
	
	for(int i = 16; i > 0; --i)
	{
		projectedCoord = uProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;
		
		testPosition = texture(uTexPosition, projectedCoord.xy);
		dDepth = rayPosition.z - testPosition.w;

		dir *= 0.5;
		if(dDepth > 0.0)
			rayPosition -= dir;
		else
			rayPosition += dir;
	}
	
	return projectedCoord.xy;
}

vec2 ssrRayMarch(const vec4 position, const vec3 reflection)
{
	vec4 projectedCoord;
	
	// convert to view space
	vec3 rayPosition = (uView * vec4(position.xyz, 1.0)).xyz;
	vec3 viewReflection = normalize((uView * vec4(reflection, 0.0)).xyz);
	
	// step 0.5m
	vec3 dir = viewReflection * 0.5;
	
	vec2 ssrUV;
	
	const vec2 uvOffset = vec2(0.5, 0.5);
	const vec2 uvScale = vec2(0.5, 0.5);
	
	vec4 testPosition;
	float depthDiff;
	
	// rayMarch test
	for (int i = 32; i > 0; --i)
	{
		rayPosition += dir;
		
		// convert 3d position to 2d texture coord
		projectedCoord = uProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		ssrUV = uvScale * projectedCoord.xy + uvOffset;
		
		testPosition = texture(uTexPosition, ssrUV);
		
		depthDiff = rayPosition.z - testPosition.w;
		if(depthDiff >= 0.0)
		{
			ssrUV = binarySearch(dir, rayPosition);
			return ssrUV;
		}
	}
	
	return ssrUV;
}

vec3 SSR(const vec3 baseColor, const vec4 position, const vec3 reflection, const float roughness)
{
	vec2 ssrUV = ssrRayMarch(position, reflection);
	
	float mipLevel = roughness * 5.0;
	
	// z clip when camera look down
	float z = (uView * vec4(reflection, 0.0)).z;
	z = clamp(z, 0.0, 1.0);
	
	// convert 3d position to 2d texture coord
	vec3 color = textureLod(uTexLastFrame, ssrUV, mipLevel).rgb;
	
	// edge factor
	vec2 dCoords = smoothstep(vec2(0.0, 0.0), vec2(0.5, 0.5), abs(vec2(0.5, 0.5) - ssrUV));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	return mix(baseColor * 0.8, color, screenEdgefactor * z);
}