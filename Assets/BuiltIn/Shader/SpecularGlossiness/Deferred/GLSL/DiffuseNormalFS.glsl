precision highp float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;

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
	vec3 baseMap = texture(uTexDiffuse, vTexCoord0.xy).rgb;
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;

	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y = localCoords.y * vTangentW;
	vec3 n = rotation * localCoords;
	n = normalize(n);

#ifdef INSTANCING
	Diffuse = vec4(baseMap * vColor.rgb, 1.0);
#else
	Diffuse = vec4(baseMap * uColor.rgb, 1.0);
#endif
	
	Position = vWorldPosition;
	Normal = vec4(n, 1.0);
	
#ifdef INSTANCING
	SG = vec4(vSpecGloss.x, vSpecGloss.y, 1.0, 1.0);
#else	
	SG = vec4(uSpecGloss.x, uSpecGloss.y, 1.0, 1.0);
#endif
}
