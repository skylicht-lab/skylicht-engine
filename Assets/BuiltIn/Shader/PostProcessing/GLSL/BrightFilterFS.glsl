precision mediump float;

uniform sampler2D uSourceTex;

uniform vec4 uCurve;

in vec2	varTexCoord0;

out vec4 FragColor;

float brightness(vec3 c)
{
	return max(max(c.r, c.g), c.b);
}

void main(void)
{
	vec3 m = texture(uSourceTex, varTexCoord0.xy).rgb;
	float br = brightness(m);

	// Under-threshold part: quadratic curve
	float rq = clamp(br - uCurve.x, 0, uCurve.y);
	rq = uCurve.z * rq * rq;

	// Combine and apply the brightness response curve.
	m *= max(rq, br - uCurve.w) / max(br, 1e-5);

	FragColor = vec4(m, 1.0);
}