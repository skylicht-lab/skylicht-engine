precision highp float;
uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;
in vec4 varColor;
in vec2 varTexCoord0;
out vec4 FragColor;
float hash( in vec2 p )
{
    p = 17.0*fract( p*0.3183099 + vec2(0.71,0.113));
    return -1.0+2.0*fract( p.x*p.y*(p.x+p.y) );
}
vec3 noised( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    vec2 du = 30.0*f*f*(f*(f-2.0)+1.0);
    float va = hash( i + vec2(0.0,0.0) );
    float vb = hash( i + vec2(1.0,0.0) );
    float vc = hash( i + vec2(0.0,1.0) );
    float vd = hash( i + vec2(1.0,1.0) );
    float k0 = va;
    float k1 = vb - va;
    float k2 = vc - va;
    float k4 = va - vb - vc + vd;
    return vec3( va+(vb-va)*u.x+(vc-va)*u.y+(va-vb-vc+vd)*u.x*u.y,
                 du*(u.yx*(va-vb-vc+vd) + vec2(vb,vc) - va) );
}
float noise( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );
	vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( hash( i + vec2(0.0,0.0) ),
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ),
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}
float pnoise( vec2 p )
{
	mat2 m = mat2( 1.6, 1.2, -1.2, 1.6 );
	float f = 0.5000*noise( p ); p = m*p;
	f += 0.2500*noise( p ); p = m*p;
	f += 0.1250*noise( p ); p = m*p;
	f += 0.0625*noise( p ); p = m*p;
	return f;
}
void main(void)
{
	float f = pnoise(uNoiseOffset.xy + varTexCoord0 * uNoiseOffset.w);
	float t = 0.2;
	float x = varTexCoord0.x * 2.0 - 1.0;
	f = abs(f * t + x + 0.01);
	f = pow(f, 0.2);
	vec3 col = vec3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;
	FragColor = varColor * vec4(col, 1.0);
}
