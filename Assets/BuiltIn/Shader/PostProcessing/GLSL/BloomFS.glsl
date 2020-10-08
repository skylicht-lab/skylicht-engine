precision mediump float;

uniform sampler2D uSourceTex;
uniform sampler2D uBlurTex;

uniform float uBloomIntensity;

in vec2	varTexCoord0;

out vec4 FragColor;

void main(void)
{
	// base color
	vec3 base = texture(uSourceTex, varTexCoord0).rgb;

	// blur color
	vec3 blur = texture(uBlurTex, varTexCoord0).rgb;
	
	vec3 m = base + blur * uBloomIntensity;

	FragColor = vec4(m, 1.0);
}