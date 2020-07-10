precision mediump float;

uniform sampler2D uTexNormal;
uniform samplerCube uTexReflect;

in vec4 vColor;
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
in vec4 vViewPosition;
in vec3 vWorldPosition;

out vec4 FragColor;

const float PI = 3.1415926;

void main(void)
{
	vec3 diffuseColor = vec3(0.5, 0.5, 0.5);
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	
	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y *= vTangentW;
	vec3 n = normalize(rotation * localCoords);
	n = normalize(n);
	
	vec3 color = vec3(0.0, 0.0, 0.0);
	
	// Specular
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);	
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 100.0f * 0.5) * 0.5;
	color += vec3(specular, specular, specular);
	
	// Reflection
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));
	color += textureLod(uTexReflect, reflection, 0.0).xyz;
	
	FragColor = vec4(color, 1.0);
}