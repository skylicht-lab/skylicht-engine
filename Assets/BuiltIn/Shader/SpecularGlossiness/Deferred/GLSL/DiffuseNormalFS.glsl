precision highp float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;

uniform float uSpec;
uniform float uGloss;

in vec2 vTexCoord0;
in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

void main(void)
{
	vec3 baseMap = texture(uTexDiffuse, vTexCoord0.xy).xyz;
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;

	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0);
	localCoords.y = localCoords.y * vTangentW;
	vec3 n = rotation * localCoords;
	n = normalize(n);

	Diffuse = vec4(baseMap, 1.0);
	Position = vec4(vWorldPosition, 1.0);
	Normal = vec4(n, 1.0);
	SG = vec4(uSpec, uGloss, 1.0, 1.0);
}
