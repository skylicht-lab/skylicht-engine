precision mediump float;

uniform sampler2D uTexDiffuse1;

#if defined(UV2) || defined(LAYER2)
uniform sampler2D uTexDiffuse2;
#endif

in vec2 varTexCoord0;
#ifdef UV2
in vec2 varTexCoord1;
#endif
in vec4 varUVScale;
#ifdef UV2
in vec4 varUVScale1;
#endif
in vec4 varColor;
#ifdef RIM_LIGHT
in vec3 varWorldNormal;
in vec3 varWorldViewDir;
#endif

uniform vec4 uColor1;
#if defined(UV2) || defined(LAYER2)	
uniform vec4 uColor2;
#endif
uniform vec4 uColorIntensity;
uniform vec4 uTime;

out vec4 FragColor;

void main(void)
{
	vec2 tex0 = varTexCoord0 * varUVScale.xy + varUVScale.zw;

#ifdef UV2
	vec2 tex1 = varTexCoord1 * varUVScale1.xy + varUVScale1.zw;
#endif

#ifdef SCROLL_Y
	vec2 uvOffset = vec2(0.0, uTime.y) / 4.0;
#else
	vec2 uvOffset = vec2(-uTime.y, 0.0) / 4.0;
#endif

#ifdef SCROLL_UV1
	vec4 color1 = texture(uTexDiffuse1, tex0 + uvOffset);
#elif defined(NO_TILE_UV1)
	vec4 color1 = texture(uTexDiffuse1, varTexCoord0);
#else	
	vec4 color1 = texture(uTexDiffuse1, tex0);
#endif

#if defined(UV2)
// use 2 uv
#ifdef SCROLL_UV2
	vec4 color2 = texture(uTexDiffuse2, tex1 + uvOffset);
#else
	vec4 color2 = texture(uTexDiffuse2, tex1);
#endif
#elif defined(LAYER2)
// use 1 uv
#ifdef SCROLL_LAYER2
	vec4 color2 = texture(uTexDiffuse2, tex0 + uvOffset);
#else
	vec4 color2 = texture(uTexDiffuse2, tex0);
#endif
#endif

#ifdef RIM_LIGHT
	float VdotN = max(dot(varWorldViewDir, varWorldNormal), 0.0);
	float rimLightIntensity = max(0.0, 1.0 - VdotN);
	rimLightIntensity = pow(rimLightIntensity, 0.9);
#endif

#if defined(UV2) || defined(LAYER2)
	vec4 result = (color1 * uColor1 + color2 * uColor2) * varColor * uColorIntensity;
	result.a = color1.a * color2.a * uColor1.a * uColor2.a;
#ifdef RIM_LIGHT
	result.a = result.a * rimLightIntensity;
#endif	
	FragColor = result;
#else
	FragColor = color1 * varColor * uColor1 * uColorIntensity;
#endif
}