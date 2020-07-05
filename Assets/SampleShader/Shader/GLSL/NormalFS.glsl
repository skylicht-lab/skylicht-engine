precision mediump float;

uniform sampler2D uTexNormal;

uniform vec4 uLightColor;
uniform vec4 uSHConst[4];

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
	
	// SH Ambient
	vec3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	
	// Lighting
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * uLightColor.rgb;
	vec3 color = directionalLight * diffuseColor;

	// Specular
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);	
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 100.0f * 0.5) * 0.5;
	color += vec3(specular, specular, specular);
	
	// IBL lighting (2 bounce)
	color += ambientLighting * diffuseColor * 1.5 / PI;
	
	FragColor = vec4(color, 1.0);
}