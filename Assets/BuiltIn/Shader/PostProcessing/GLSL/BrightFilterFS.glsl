precision mediump float;

uniform sampler2D uSourceTex;
uniform sampler2D uSourceEmission;

uniform vec4 uCurve;

in vec2	varTexCoord0;

out vec4 FragColor;

void main(void)
{
	vec3 m = texture(uSourceTex, varTexCoord0.xy).rgb;
	m = min(m, vec3(4.0, 4.0, 4.0));
	
	vec3 e = texture(uSourceEmission, varTexCoord0.xy).rgb;
	
	// brightness
	float br = max(max(m.r, m.g), m.b);

	// Under-threshold part: quadratic curve
	float rq = clamp(br - uCurve.x, 0.0, uCurve.y);
	rq = uCurve.z * rq * rq;

	// Combine and apply the brightness response curve.
	m *= max(rq, br - uCurve.w) / max(br, 1e-5);

	FragColor = vec4(m + e, 1.0);
}