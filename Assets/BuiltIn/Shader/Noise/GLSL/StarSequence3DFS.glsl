precision mediump float;
uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;
in vec4 varColor;
in vec4 varWorldPos;
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
const float gamma = 2.2;
const float invGamma = 1.0/2.2;
vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}
vec3 linearRGB(vec3 color)
{
	return pow(color, vec3(invGamma));
}
float snoise(vec3 coord)
{
	return 1.0 - noise(coord) * 2.0;
}
void main(void)
{
	vec3 coord = varWorldPos.xyz * uNoiseOffset.w;
	float n = pnoise(uNoiseOffset.xyz + varWorldPos.xyz * 12.0);
	coord += vec3(n, 0.0, n);
	float color = 0.5;
	float scale = 1.3;
	int i = 1;
	{
        float power = pow(2.0, i);
		float res = power * scale;
        color += (0.4 / power) * snoise(coord * res);
    }
	i++;
	{
        float power = pow(2.0, i);
		float res = power * scale;
        color += (0.4 / power) * snoise(coord * res);
    }
	color *= 2.3;
	float r = color;
	float g = pow(max(color, 0.0),2.0)*0.4;
	float b = pow(max(color, 0.0),3.0)*0.15;
	vec4 ret = varColor * vec4(r, g, b, 1.0);
	FragColor = vec4(sRGB(ret.rgb), ret.a);
}
