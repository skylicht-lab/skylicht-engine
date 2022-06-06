// References
// https://github.com/MonkeyFirst/Urho3D_SSR_Shader/blob/master/bin/CoreData/Shaders/GLSL/SSR.glsl
vec2 binarySearch(vec3 dir, vec3 rayPosition)
{
	vec4 projectedCoord;
	
	for(int i = 16; i > 0; --i)
	{
		projectedCoord = uProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		projectedCoord.xy = 0.5 * projectedCoord.xy + vec2(0.5, 0.5);
		
		vec4 testPosition = texture(uTexPosition, projectedCoord.xy);
		float dDepth = rayPosition.z - testPosition.w;

		dir *= 0.5;
		if(dDepth > 0.0)
			rayPosition -= dir;
		else
			rayPosition += dir;
	}
	
	return projectedCoord.xy;
}

vec3 SSR(const vec3 baseColor, const vec4 position, const vec3 reflection, const float roughness)
{
	vec4 projectedCoord;
	
	// convert to view space
	vec3 rayPosition = (uView * vec4(position.xyz, 1.0)).xyz;
	vec3 viewReflection = normalize((uView * vec4(reflection, 0.0)).xyz);
	
	// step 0.5m
	vec3 dir = viewReflection * 0.5;
	
	float mipLevel = roughness * 5.0;
	vec2 ssrUV;
	
	// ray test
	for (int i = 32; i > 0; --i)
	{
		rayPosition += dir;
		
		// convert 3d position to 2d texture coord
		projectedCoord = uProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		ssrUV = 0.5 * projectedCoord.xy + vec2(0.5, 0.5);
		
		vec4 testPosition = texture(uTexPosition, ssrUV);
		
		float depthDiff = rayPosition.z - testPosition.w;
		if(depthDiff >= 0.0)
		{
			ssrUV = binarySearch(dir, rayPosition);
			break;
		}
	}
	
	// z clip when camera look down
	float z = (uView * vec4(reflection, 0.0)).z;
	z = clamp(z, 0.0, 1.0);
	
	// convert 3d position to 2d texture coord
	vec3 color = textureLod(uTexLastFrame, ssrUV, mipLevel).rgb;
	
	// edge factor
	vec2 dCoords = smoothstep(vec2(0.0, 0.0), vec2(0.5, 0.5), abs(vec2(0.5, 0.5) - ssrUV));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	return mix(baseColor * 0.8, color, screenEdgefactor * z);
	// return vec3(screenEdgefactor * z, screenEdgefactor * z, screenEdgefactor * z);
}