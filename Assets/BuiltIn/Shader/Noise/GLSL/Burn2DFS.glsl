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
	float color = 1.0 - varTexCoord0.y + 0.4;
    vec3 coord = vec3(varTexCoord0.x/6.0, varTexCoord0.y/6.0, 0.5);
    coord = 1.0 - coord;
	float speedMul = 0.05;
	vec3 noiseOffset = vec3(0.0, uNoiseOffset.x, uNoiseOffset.x) * speedMul;
    int i = 1;
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
    color = 1.0 - color;
    color *= 2.7;
	vec3 col = vec3(color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.0)*0.15);
	FragColor = varColor * vec4(col, 1.0);
}
