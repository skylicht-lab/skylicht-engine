#include "../../Shadow/GLSL/LibShadowSimple.glsl"

// https://learnopengl.com/Guest-Articles/2022/Area-Lights
vec3 integrateEdge(vec3 v1, vec3 v2) 
{
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

vec3 arealightShadow(
	const vec3 position, 
	const vec3 normal,
	const vec3 camPosition, 
	const vec4 lightColor, 
	const vec3 lightPosition, 
	const vec3 lightDirX,
	const vec3 lightDirY,
	const vec2 lightSize)
{
	vec3 v0 = lightPosition - lightDirX * lightSize.x - lightDirY * lightSize.y;
	vec3 v1 = lightPosition + lightDirX * lightSize.x - lightDirY * lightSize.y;
    vec3 v2 = lightPosition + lightDirX * lightSize.x + lightDirY * lightSize.y;
    vec3 v3 = lightPosition - lightDirX * lightSize.x + lightDirY * lightSize.y;
	
    vec3 L[4];
    L[0] = (v0 - position);
    L[1] = (v1 - position);
    L[2] = (v2 - position);
    L[3] = (v3 - position);
	
	L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);
	
	vec3 sum = vec3(0.0, 0.0, 0.0);
	sum += integrateEdge(L[0], L[1]);
	sum += integrateEdge(L[1], L[2]);
	sum += integrateEdge(L[2], L[3]);
	sum += integrateEdge(L[3], L[0]);
	
	float len = length(sum);
	
	vec3 lightNormal = -normalize(cross(v1 - v0, v3 - v0));   
	len = len * max(dot(normal, lightNormal), 0.0);
	
	vec4 shadowCoord = uShadowMatrix * vec4(position, 1.0);
	len = len * shadowSimple(shadowCoord);
	
	return max(0.0, len) * lightColor.rgb * lightColor.a;
}