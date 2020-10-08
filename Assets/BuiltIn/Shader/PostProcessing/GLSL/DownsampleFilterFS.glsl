precision mediump float;

uniform sampler2D uSourceTex;

uniform vec2 uTexelSize;

in vec2	varTexCoord0;

out vec4 FragColor;

float brightness(vec3 c)
{
	return max(max(c.r, c.g), c.b);
}

vec3 downsampleAntiFlickerFilter(vec2 uv)
{
	vec4 d = vec4(-uTexelSize.x, -uTexelSize.y, uTexelSize.x, uTexelSize.y);

	vec3 s1 = texture(uSourceTex, uv + d.xy).rgb;
	vec3 s2 = texture(uSourceTex, uv + d.zy).rgb;
	vec3 s3 = texture(uSourceTex, uv + d.xw).rgb;
	vec3 s4 = texture(uSourceTex, uv + d.zw).rgb;

	// Karis's luma weighted average (using brightness instead of luma)
	float s1w = 1.0 / (brightness(s1) + 1.0);
	float s2w = 1.0 / (brightness(s2) + 1.0);
	float s3w = 1.0 / (brightness(s3) + 1.0);
	float s4w = 1.0 / (brightness(s4) + 1.0);
	float oneDivWSum = 1.0 / (s1w + s2w + s3w + s4w);

	return (s1 * s1w + s2 * s2w + s3 * s3w + s4 * s4w) * oneDivWSum;
}

void main(void)
{
	vec3 m = downsampleAntiFlickerFilter(varTexCoord0);

	FragColor = vec4(m, 1.0);
}