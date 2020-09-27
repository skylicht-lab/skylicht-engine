struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerFrame
{
	float4 uNoiseOffset;
	float4 uExplosionParams;
};
float hash( in float2 p )
{
    p = 50.0*frac( p*0.3183099 + float2(0.71,0.113));
    return -1.0+2.0*frac( p.x*p.y*(p.x+p.y) );
}
float3 noised( in float2 p )
{
    float2 i = floor( p );
    float2 f = frac( p );
    float2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    float2 du = 30.0*f*f*(f*(f-2.0)+1.0);
    float va = hash( i + float2(0.0,0.0) );
    float vb = hash( i + float2(1.0,0.0) );
    float vc = hash( i + float2(0.0,1.0) );
    float vd = hash( i + float2(1.0,1.0) );
    float k0 = va;
    float k1 = vb - va;
    float k2 = vc - va;
    float k4 = va - vb - vc + vd;
    return float3( va+(vb-va)*u.x+(vc-va)*u.y+(va-vb-vc+vd)*u.x*u.y,
                 du*(u.yx*(va-vb-vc+vd) + float2(vb,vc) - va) );
}
float noise( in float2 p )
{
    float2 i = floor( p );
    float2 f = frac( p );
	float2 u = f*f*(3.0-2.0*f);
    return lerp( lerp( hash( i + float2(0.0,0.0) ),
                     hash( i + float2(1.0,0.0) ), u.x),
                lerp( hash( i + float2(0.0,1.0) ),
                     hash( i + float2(1.0,1.0) ), u.x), u.y);
}
float pnoise( float2 p )
{
	float2x2 m = float2x2( 1.6, 1.2, -1.2, 1.6 );
	float f = 0.5000*noise( p );
	p = mul(p, m);
	f += 0.2500*noise( p );
	p = mul(p, m);
	f += 0.1250*noise( p );
	p = mul(p, m);
	f += 0.0625*noise( p );
	p = mul(p, m);
	return f;
}
float3 heatToColor(float heat)
{
    float3 col = lerp(
		float3(0.0, 0.0, 0.0),
		float3(1.0, 0.3, 0.0),
		clamp(heat * 15.0 - 2.0, 0.0, 1.0));
    col = lerp(col,
		float3(1.0, 1.0, 0.6),
		clamp(heat * 15.1 - 4.0, 0.0, 1.0));
    col = lerp(col,
		float3(1.0, 0.9, 0.8),
		clamp(heat * 190.0 - 60.0, 0.0, 1.0));
    return col;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float f = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);
	float t = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w * 1.3);
	f = 0.5 + 0.5*f;
	t = 0.5 + 0.5*t;
	float a = f;
	f = f * uExplosionParams.x;
	float heat = f * (0.4 - (1.0 - t) * 0.2);
	float3 color = heatToColor(heat);
	float3 scolor = float3(0.0, 0.0, 0.0);
	float smoke = heat/0.03;
	if (smoke < 1.0)
	{
		float s = smoke*0.5;
		scolor = float3(s, s, s);
	}
	color = lerp(color, scolor, f);
	color = smoothstep(
		float3(0.0, 0.0, 0.0),
		float3(0.3, 0.3, 0.3),
		color);
	a = a * uExplosionParams.y;
	a = smoothstep(0.0, 0.95, a);
	return input.color * float4(color, a);
}
