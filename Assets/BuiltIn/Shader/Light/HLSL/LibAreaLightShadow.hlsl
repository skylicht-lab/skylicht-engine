#include "../../Shadow/HLSL/LibShadowSimple.hlsl"

// https://learnopengl.com/Guest-Articles/2022/Area-Lights
float3 integrateEdge(float3 v1, float3 v2) 
{
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*rsqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

float3 arealightShadow(const float3 position, 
	const float3 normal,
	const float3 camPosition, 
	const float4 lightColor, 
	const float3 lightPosition, 
	const float3 lightDirX,
	const float3 lightDirY,
	const float2 lightSize)
{
	float3 v0 = lightPosition - lightDirX * lightSize.x - lightDirY * lightSize.y;
	float3 v1 = lightPosition + lightDirX * lightSize.x - lightDirY * lightSize.y;
    float3 v2 = lightPosition + lightDirX * lightSize.x + lightDirY * lightSize.y;
    float3 v3 = lightPosition - lightDirX * lightSize.x + lightDirY * lightSize.y;
	
    float3 L[4];
    L[0] = (v0 - position);
    L[1] = (v1 - position);
    L[2] = (v2 - position);
    L[3] = (v3 - position);
	
	L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);
	
	float3 sum = float3(0.0, 0.0, 0.0);
	sum += integrateEdge(L[0], L[1]);
	sum += integrateEdge(L[1], L[2]);
	sum += integrateEdge(L[2], L[3]);
	sum += integrateEdge(L[3], L[0]);
	
	float len = length(sum);
	
	float3 dir = v0 - position;
    float3 lightNormal = cross(v1 - v0, v3 - v0);
    
	float t = step(dot(dir, lightNormal), 0);
	len = len * t;
	
	float4 shadowCoord = mul(float4(position, 1.0), uShadowMatrix);
	len = len * shadowSimple(shadowCoord);
	
	return max(0.0, len) * lightColor.rgb * lightColor.a;
}