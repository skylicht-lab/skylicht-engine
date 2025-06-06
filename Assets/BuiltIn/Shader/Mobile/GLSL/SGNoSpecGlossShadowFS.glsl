// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.glsl : NO_NORMAL_MAP,NO_SPECGLOSS,SHADOW,OPTIMIZE_SHADOW]
precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexDiffuse;
uniform sampler2DArray uShadowMap;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec2 uLightMul;
uniform vec2 uSpecGloss;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec3 vDepth;
in vec4 vShadowCoord;
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
float shadow(const vec4 shadowCoord, const float farDistance)
{
	int id = 0;
	float depth = 0.0;
	const float bias = 0.0001;
	vec3 shadowUV = shadowCoord.xyz / shadowCoord.w;
	if (shadowUV.z > 1.0)
		return 1.0;
	depth = shadowUV.z;
	depth -= bias;
	vec2 uv = shadowUV.xy;
	return (step(depth, textureLod(uShadowMap, vec3(uv, id), 0.0).r));
}
const float PI = 3.1415926;
void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy) * uColor;
	vec3 specMap = vec3(uSpecGloss, 1.0);
	vec3 n = vWorldNormal;
	float depth = length(vDepth);
	float visibility = shadow(vShadowCoord, depth);
	vec3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	vec3 diffuseColor = sRGB(diffuseMap.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);
	float spec = specMap.r;
	float gloss = specMap.g;
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = directionalLight * diffuseColor * 0.3 * uLightMul.y;
	vec3 specularColor = vec3(0.5, 0.5, 0.5);
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;
	color *= visibility;
	color += ambientLighting * diffuseColor / PI;
	FragColor = vec4(color, diffuseMap.a);
}
