in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;
uniform vec4 uTime;
uniform vec4 uData; // Speed;Offset;Width

out vec2 vTexCoord0;
out vec2 vTexCoord1;
out vec4 vColor;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	vTexCoord1 = vec2(uData.z * (uData.y + inNormal.x + uTime.x * uData.x), inNormal.y);
	vColor = inColor/ 255.0;
	gl_Position = uMvpMatrix * inPosition;
}