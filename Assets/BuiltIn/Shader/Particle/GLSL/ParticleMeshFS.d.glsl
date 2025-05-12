precision highp float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;
in vec3 varWorldNormal;
in vec3 varVertexPos;

out vec4 FragColor;

uniform vec4 uColorIntensity;

#ifdef DISSOLVE
uniform vec4 uNoiseScale;
uniform vec4 uDissolveColor;
#endif

#ifdef LIGHTING
uniform vec4 uLightColor;
uniform vec4 uLightDirection;	
uniform vec4 uSHConst[4];
#endif

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

#ifdef LIGHTING
#include "../../SHAmbient/GLSL/SHAmbient.glsl"
const float PI = 3.1415926;
#endif

#ifdef DISSOLVE
#include "../../Noise/GLSL/LibNoise.glsl"
#endif

void main(void)
{
#ifdef SOILD_COLOR
	vec4 texColor = vec4(varColor.rgb, 1.0f);
#else
	vec4 texColor = texture(uTexture, varTexCoord0.xy);
#endif

#if defined(SOILD)
	vec3 color = sRGB(texColor.rgb * varColor.rgb);
#elif defined(SOILD_COLOR)
	vec3 color = sRGB(texColor.rgb);
#else
	vec3 color = sRGB(texColor.rgb * varColor.rgb * uColorIntensity.rgb);
#endif
	float alpha = texColor.a * varColor.a;
	
#ifdef DISSOLVE
	float n = pnoise(varVertexPos * uNoiseScale.xyz);
	n = 0.5 + 0.5*n;
	
	float dissolve = n - (1.0 - alpha);
	if (dissolve <= 0.0)
		discard;
#endif
	
#ifdef LIGHTING
	// Direction lighting
	float NdotL = max(dot(varWorldNormal, uLightDirection.xyz), 0.0);
	vec3 directionalLight = NdotL * sRGB(uLightColor.rgb);
	color = directionalLight * color * 0.3;
	
	// SH4 Ambient
	vec3 ambientLighting = shAmbient(varWorldNormal);	
	color += sRGB(ambientLighting * texColor.rgb) / PI;
#endif	
	
#ifdef DISSOLVE
	color += sRGB(uDissolveColor.rgb * uColorIntensity.rgb) * step(dissolve, uNoiseScale.w) * n;
#endif	
	
	FragColor = vec4(color, alpha);
}