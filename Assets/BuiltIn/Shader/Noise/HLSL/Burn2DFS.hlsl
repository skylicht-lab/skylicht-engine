struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerFrame
{
	float4 uNoiseOffset;
	float4 uElectricColor;
};
float hash(float3 p)
{
    p = frac( p*0.3183099+.1 );
	p *= 17.0;
    return frac( p.x*p.y*p.z*(p.x+p.y+p.z) );
}
float noise( in float3 x )
{
    float3 i = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    return lerp(lerp(lerp( hash(i+float3(0,0,0)),
                        hash(i+float3(1,0,0)),f.x),
                   lerp( hash(i+float3(0,1,0)),
                        hash(i+float3(1,1,0)),f.x),f.y),
               lerp(lerp( hash(i+float3(0,0,1)),
                        hash(i+float3(1,0,1)),f.x),
                   lerp( hash(i+float3(0,1,1)),
                        hash(i+float3(1,1,1)),f.x),f.y),f.z);
}
static const float3x3 m = float3x3( 0.00, 0.80, 0.60,
                    -0.80, 0.36, -0.48,
                    -0.60, -0.48, 0.64 );
float pnoise( float3 q )
{
	float f = 0.5000*noise( q );
	q = mul(q, m)*2.01;
	f += 0.2500*noise( q );
	q = mul(q, m)*2.02;
	f += 0.1250*noise( q );
	q = mul(q, m)*2.03;
	f += 0.0625*noise( q );
	q = mul(q, m)*2.01;
	return f;
}
float fbm(float3 p)
{
	float z=2.;
	float rz = 0.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	rz+= abs((noise(p)-0.5)*2.)/z;
	z = z*2.;
	p = p*2.;
	return rz;
}
float snoise( in float3 x )
{
	return noise(x) * 2.0 - 1.0;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	int ITERATIONS = 13;
	float uSPEED = 10.0;
	float uDISPLACEMENT = 0.05;
	float uTIGHTNESS = 10.0;
	float uYOFFSET = 0.3;
	float uYSCALE = 0.2;
	float uXSCALE = 0.5;
	float3 uFLAMETONE = float3(50.0, 5.0, 1.0);
	float nx = 0.0;
	float ny = 0.0;
	float time = uNoiseOffset.x;
	float2 uv = input.tex0;
	float2 uvNoise = input.tex0 * float2(uXSCALE, uYSCALE);
	for (int i=4; i<ITERATIONS+1; i++)
	{
		float ii = pow((float)i, 2.0);
		float ifrac = float(i)/float(ITERATIONS);
		float t = ifrac * time * uSPEED;
		float d = (1.0-ifrac) * uDISPLACEMENT;
		nx += snoise( float3(uvNoise.x*ii-time*ifrac, uvNoise.y*ii-t, 0.0)) * d * 2.0;
		ny += snoise( float3(uvNoise.x*ii+time*ifrac, uvNoise.y*ii-t, time*ifrac/ii)) * d;
	}
	float2 pos = float2(uv.x+nx, uv.y+ny);
	float flame = clamp(sin(pos.x*3.1416) - pos.y+uYOFFSET, 0.0, 1.0);
	float3 col = pow(flame, uTIGHTNESS) * uFLAMETONE;
    col = col / (1.0+col);
	float e = 1.0/2.2;
    col = pow(col, float3(e, e, e));
    col = clamp(col, 0.0, 1.0);
	return input.color * float4(col, 1.0);
}
