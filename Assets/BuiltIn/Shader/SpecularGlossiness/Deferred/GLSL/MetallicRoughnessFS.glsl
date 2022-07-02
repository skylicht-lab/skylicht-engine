precision highp float;

uniform sampler2D uTexAlbedo;
uniform sampler2D uTexNormal;
uniform sampler2D uTexRoughMetal;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;

uniform vec4 uColor;

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

void main(void)
{
	vec3 baseMap = texture(uTexAlbedo, vTexCoord0.xy).rgb * uColor.rgb;
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	vec3 rmMap = texture(uTexRoughMetal, vTexCoord0.xy).rgb;

	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y = localCoords.y * vTangentW;
	vec3 n = rotation * localCoords;
	n = normalize(n);

	// convert (base rough metal) to (diff spec gloss)
	// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/a94655275e5e4e8ae580b1d95ce678b74ab87426/shaders/pbr-frag.glsl
	// line 220
	float roughness = rmMap.r;
	float metallic = rmMap.g;
	vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseMap.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    vec3 specularColor = mix(f0, baseMap.rgb, max(roughness, metallic));

	Diffuse = vec4(diffuseColor, 1.0);
	Position = vWorldPosition;
	Normal = vec4(n, 1.0);
	SG = vec4(max(specularColor.r, specularColor.g), 1.0 - roughness, 1.0, 1.0);
}
