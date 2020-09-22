precision highp float;
uniform vec4 uNoiseOffset;
in vec4 varColor;
in vec2 varTexCoord0;
out vec4 FragColor;
float hash(vec3 p)
{
    p = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}
float noise( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    return mix(mix(mix( hash(i+vec3(0,0,0)),
                        hash(i+vec3(1,0,0)),f.x),
                   mix( hash(i+vec3(0,1,0)),
                        hash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(i+vec3(0,0,1)),
                        hash(i+vec3(1,0,1)),f.x),
                   mix( hash(i+vec3(0,1,1)),
                        hash(i+vec3(1,1,1)),f.x),f.y),f.z);
}
const mat3 m = mat3( 0.00, 0.80, 0.60,
                    -0.80, 0.36, -0.48,
                    -0.60, -0.48, 0.64 );
float pnoise( vec3 q )
{
	float f = 0.5000*noise( q ); q = m*q*2.01;
	f += 0.2500*noise( q ); q = m*q*2.02;
	f += 0.1250*noise( q ); q = m*q*2.03;
	f += 0.0625*noise( q ); q = m*q*2.01;
	return -1.0 + f * 2.0;
}
float snoise( in vec3 x )
{
	return noise(x) * 2.0 - 1.0;
}
void main(void)
{
	int ITERATIONS = 13;
	float uSPEED = 10.0;
	float uDISPLACEMENT = 0.05;
	float uTIGHTNESS = 10.0;
	float uYOFFSET = 0.3;
	float uYSCALE = 0.2;
	float uXSCALE = 0.5;
	vec3 uFLAMETONE = vec3(50.0, 5.0, 1.0);
	float nx = 0.0;
	float ny = 0.0;
	float time = uNoiseOffset.x;
	vec2 uv = varTexCoord0;
	vec2 uvNoise = varTexCoord0 * vec2(uXSCALE, uYSCALE);
	for (int i=4; i<ITERATIONS+1; i++)
	{
		float ii = pow(i, 2.0);
		float ifrac = float(i)/float(ITERATIONS);
		float t = ifrac * time * uSPEED;
		float d = (1.0-ifrac) * uDISPLACEMENT;
		nx += snoise( vec3(uvNoise.x*ii-time*ifrac, uvNoise.y*ii-t, 0.0)) * d * 2.0;
		ny += snoise( vec3(uvNoise.x*ii+time*ifrac, uvNoise.y*ii-t, time*ifrac/ii)) * d;
	}
	vec2 pos = vec2(uv.x+nx, uv.y+ny);
	float flame = clamp(sin(pos.x*3.1416) - pos.y+uYOFFSET, 0.0, 1.0);
	vec3 col = pow(flame, uTIGHTNESS) * uFLAMETONE;
    col = col / (1.0+col);
	float e = 1.0/2.2;
    col = pow(col, vec3(e, e, e));
    col = clamp(col, 0.0, 1.0);
	FragColor = varColor * vec4(col, 1.0);
}
