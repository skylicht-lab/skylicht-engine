in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;

out vec2 vTexCoord0;
out vec2 vTexCoord1;
out vec4 vColor;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	vTexCoord1 = vec2(inNormal.x, inNormal.y);
	vColor = inColor;
	gl_Position = uMvpMatrix * inPosition;
}