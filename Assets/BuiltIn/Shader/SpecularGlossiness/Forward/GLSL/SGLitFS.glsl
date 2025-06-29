// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.glsl : _]
precision mediump float;
uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;
uniform sampler2D uTexSpecular;
uniform samplerCube uTexReflect;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec2 uLightMul;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
in vec4 vViewPosition;
out vec4 FragColor;
const float gamma = 2.2;
const float invGamma = 1.0 / 2.2;
vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}
vec3 linearRGB(vec3 color)
{
	return pow(color, vec3(invGamma));
}
vec3 shAmbient(vec3 n)
{
	vec3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	return ambientLighting * 0.75;
}
const float PI = 3.1415926;
const float MinReflectance = 0.04;
float getPerceivedBrightness(vec3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}
float solveMetallic(vec3 diffuse, vec3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}
void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy) * uColor;
	vec3 specMap = texture(uTexSpecular, vTexCoord0.xy).xyz;
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y *= vTangentW;
	vec3 n = normalize(rotation * localCoords);
	float spec = specMap.r;
	float gloss = specMap.g;
	float roughness = 1.0 - gloss;
	vec3 diffuseColor = diffuseMap.rgb;
	vec3 f0 = vec3(spec, spec, spec);
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(diffuseColor, f0, oneMinusSpecularStrength);
	f0 = vec3(0.1, 0.1, 0.1);
	vec3 specularColor = mix(f0, diffuseColor, metallic);
	vec3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	diffuseColor = sRGB(diffuseColor);
	specularColor = sRGB(specularColor);
	vec3 lightColor = sRGB(uLightColor.rgb);
	float c = (1.0 - spec * gloss);
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = (directionalLight * diffuseColor) * (0.1 + roughness * 0.3) * c * uLightMul.y;
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;
	color += ambientLighting * diffuseColor * (0.1 + c * 0.9) / PI;
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));
	float brightness = (0.8 + gloss * 1.8);
	color += sRGB(textureLod(uTexReflect, reflection, roughness * 7.0).xyz) * brightness * specularColor;
	FragColor = vec4(color, diffuseMap.a);
}
