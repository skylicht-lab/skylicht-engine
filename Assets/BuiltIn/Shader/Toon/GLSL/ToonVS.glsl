in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
uniform vec4 uUVScale;

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldPosition;
out vec3 vDepth;
out vec4 vColor;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	
	vec4 worldPos = uWorldMatrix * inPosition;	
	vec4 worldNormal = uWorldMatrix * vec4(inNormal.xyz, 0.0);
	
	vWorldPosition = worldPos.xyz;
	vDepth = uCameraPosition.xyz - vWorldPosition;
	
	vWorldNormal = normalize(worldNormal.xyz);
	vWorldViewDir = normalize(vDepth);
	
	vColor = inColor / 255.0;
	
	gl_Position = uMvpMatrix * inPosition;
}