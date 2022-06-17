precision highp float;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in float vTangentW;

#ifdef INSTANCING
in vec4 vColor;
in vec2 vSpecGloss;
#else
uniform vec4 uColor;
uniform float uSpec;
uniform float uGloss;
#endif

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

void main(void)
{
#ifdef INSTANCING
	Diffuse = vColor;
	SG = vec4(vSpecGloss.x, vSpecGloss.y, 0.0, 1.0);
#else
	Diffuse = uColor;
	SG = vec4(uSpec, uGloss, 0.0, 1.0);
#endif
	Position = vWorldPosition;
	Normal = vec4(vWorldNormal, 1.0);
}
