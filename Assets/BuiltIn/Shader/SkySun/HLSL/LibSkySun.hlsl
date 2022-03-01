float3 GetSkyColor(
	float3 viewDir, 
	float3 sunDirection,
	float intensity,
	float4 atmosphericColor,
	float4 sunColor,
	float4 glowColor1,
	float4 glowColor2,
	float sunRadius)
{
	float y = 1.0 - (max(viewDir.y, 0.0) * 0.8 + 0.2) * 0.8;

	// SKY
	float3 skyColor = float3(pow(y, 2.0), y, 0.6 + y*0.4) * intensity;
	
	// SUN
	float sunAmount = max(dot(sunDirection, viewDir), 0.0);
	skyColor += atmosphericColor.rgb * sunAmount * sunAmount * atmosphericColor.a;
	skyColor += sunColor.rgb * pow(sunAmount, sunRadius) * sunColor.a;
	
	// GLARE
	skyColor += glowColor1.rgb * pow(sunAmount, 8.0) * glowColor1.a;
	skyColor += glowColor2.rgb * pow(sunAmount, 3.0) * glowColor2.a;
	
	return skyColor;
}