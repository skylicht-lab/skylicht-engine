#define RAY_LENGTH 32.0

vec3 SSR(const vec3 baseColor, const vec3 position, const vec3 reflection, const float roughness)
{	
	vec4 projectedCoord;
	
	vec3 beginPosition;
	vec3 endPosition;
	
	vec3 rayPosition = position;
	vec4 viewPosition;
		
	vec3 dir = reflection * RAY_LENGTH;
	
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
		projectedCoord = uViewProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		projectedCoord.xy = 0.5 * projectedCoord.xy + vec2(0.5, 0.5);
		
		vec3 testPosition = texture(uTexPosition, projectedCoord.xy).xyz;
		
		vec3 d1 = testPosition - beginPosition;
		float lengthSQ1 = d1.x*d1.x + d1.y*d1.y + d1.z*d1.z;
		
		vec3 d2 = testPosition - endPosition;
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
	float z = (uView * vec4(reflection, 0.0)).z;
	z = clamp(z, 0.0, 1.0);
	
	// convert 3d position to 2d texture coord
	vec3 color = textureLod(uTexLastFrame, projectedCoord.xy, 0).rgb;
	
	// edge factor
	vec2 dCoords = smoothstep(vec2(0.0, 0.0), vec2(0.5, 0.5), abs(vec2(0.5, 0.5) - projectedCoord.xy));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	return mix(baseColor * 0.8, color, screenEdgefactor * z);
	// return vec3(screenEdgefactor * z, screenEdgefactor * z, screenEdgefactor * z);
}