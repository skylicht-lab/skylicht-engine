// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// https://www.shadertoy.com/view/4sfGzS

#define USE_PROCEDURAL

#ifdef USE_PROCEDURAL
float hash(float3 p)  // replace this by something better
{
    p  = frac( p*0.3183099+.1 );
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
#else
float noise( in float3 x )
{
    float3 i = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);
	float2 uv = (i.xy+float2(37.0,17.0)*i.z) + f.xy;
	float2 rg = uNoiseTexture.SampleLevel( uNoiseTextureSampler, (uv+0.5)/256.0, 0.0).yx;
	return lerp( rg.x, rg.y, f.z );    
}
#endif

static const float3x3 m = float3x3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );

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
		
	return -1.0 + f * 2.0;
}