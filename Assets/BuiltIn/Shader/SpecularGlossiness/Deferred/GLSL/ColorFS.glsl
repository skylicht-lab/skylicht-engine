precision highp float;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;

#ifdef TANGENT
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
#endif

#ifdef INSTANCING
in vec4 vColor;
in vec2 vSpecGloss;
#else
uniform vec4 uColor;
uniform vec2 uSpecGloss;
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
	SG = vec4(uSpecGloss.x, uSpecGloss.y, 0.0, 1.0);
#endif
	Position = vWorldPosition;
	Normal = vec4(vWorldNormal, 1.0);
}
