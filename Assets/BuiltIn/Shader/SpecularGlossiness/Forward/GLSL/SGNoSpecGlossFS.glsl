// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.glsl : NO_NORMAL_MAP, NO_SPECGLOSS]
precision mediump float;
uniform sampler2D uTexDiffuse;
uniform samplerCube uTexReflect;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec2 uSpecGloss;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
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
	return ambientLighting * 0.9;
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
	vec3 specMap = vec3(uSpecGloss, 0.0);
	vec3 n = vWorldNormal;
	float roughness = 1.0 - specMap.g;
	vec3 f0 = vec3(specMap.r, specMap.r, specMap.r);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - specMap.r;
	float metallic = solveMetallic(diffuseMap.rgb, specularColor, oneMinusSpecularStrength);
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = diffuseMap.rgb;
	specularColor = mix(f0, diffuseMap.rgb, metallic);
	vec3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	diffuseColor = sRGB(diffuseColor);
	specularColor = sRGB(specularColor);
	vec3 lightColor = sRGB(uLightColor.rgb);
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = directionalLight * diffuseColor;
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * specularColor;
	color += ambientLighting * diffuseColor / PI;
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));
	color += sRGB(textureLod(uTexReflect, reflection, roughness * 8.0).xyz) * specularColor * metallic;
	FragColor = vec4(color, diffuseMap.a);
}