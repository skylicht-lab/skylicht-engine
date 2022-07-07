in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;

out vec2 vTexCoord0;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	gl_Position = uMvpMatrix * inPosition;
}