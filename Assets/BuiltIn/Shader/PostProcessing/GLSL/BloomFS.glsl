precision mediump float;

uniform sampler2D uSourceTex;
uniform sampler2D uBlurTex;

uniform vec2 uTexelSize;

in vec2	varTexCoord0;

out vec4 FragColor;

vec3 upsampleFilter(vec2 uv)
{
    vec4 d = uTexelSize.xyxy * vec4(-1, -1, +1, +1);

    vec3 s1 = texture(uBlurTex, uv + d.xy).rgb;
    vec3 s2 = texture(uBlurTex, uv + d.zy).rgb;
    vec3 s3 = texture(uBlurTex, uv + d.xw).rgb;
    vec3 s4 = texture(uBlurTex, uv + d.zw).rgb;

    return (s1 + s2 + s3 + s4) * 0.25;
}

void main(void)
{
	// base color
	vec3 base = texture(uSourceTex, varTexCoord0).rgb;
		
	// blur color
	vec3 blur = upsampleFilter(varTexCoord0);
		
	float intensity = 1.3;
	vec3 m = base + blur * intensity;
	
	FragColor = vec4(m, 1.0);
}