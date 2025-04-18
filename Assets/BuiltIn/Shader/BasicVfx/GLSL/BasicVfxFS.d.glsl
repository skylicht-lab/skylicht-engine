precision mediump float;

uniform sampler2D uTexDiffuse1;

#if defined(UV2) || defined(LAYER2)
uniform sampler2D uTexDiffuse2;
#endif

in vec2 varTexCoord0;
#ifdef UV2
in vec2 varTexCoord1;
#endif
in vec4 varColor;

uniform vec4 uColor;
uniform vec4 uColorIntensity;
uniform vec4 uTime;

out vec4 FragColor;

void main(void)
{
#ifdef SCROLL_Y
	vec2 uvOffset = vec2(0.0, uTime.x) / 4.0;
#else
	vec2 uvOffset = vec2(uTime.x, 0.0) / 4.0;
#endif

#ifdef SCROLL_UV1
	vec4 color1 = texture(uTexDiffuse1, varTexCoord0 + uvOffset);
#else	
	vec4 color1 = texture(uTexDiffuse1, varTexCoord0);
#endif

#if defined(UV2)
// use 2 uv
#ifdef SCROLL_UV2
	vec4 color2 = texture(uTexDiffuse2, varTexCoord1 + uvOffset);
#else
	vec4 color2 = texture(uTexDiffuse2, varTexCoord1);
#endif
#elif defined(LAYER2)
// use 1 uv
#ifdef SCROLL_LAYER2
	vec4 color2 = texture(uTexDiffuse2, varTexCoord0 + uvOffset);
#else
	vec4 color2 = texture(uTexDiffuse2, varTexCoord0);
#endif
#endif

#if defined(UV2) || defined(LAYER2)
	vec4 result = (color1 + color2) * uColor * varColor * uColorIntensity;
	result.a = color1.a * color2.a * uColor.a;
	return result;
#else
	FragColor = color1 * varColor * uColor * uColorIntensity;
#endif
}