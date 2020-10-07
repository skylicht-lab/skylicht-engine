// The MIT License
// Copyright © 2017 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

float hash(in float2 p)  // replace this by something better
{
	p = 50.0*frac(p*0.3183099 + float2(0.71, 0.113));
	return -1.0 + 2.0*frac(p.x*p.y*(p.x + p.y));
}

// return value noise (in x) and its derivatives (in yz)
float3 noised(in float2 p)
{
	float2 i = floor(p);
	float2 f = frac(p);

#if 1
	// quintic interpolation
	float2 u = f * f*f*(f*(f*6.0 - 15.0) + 10.0);
	float2 du = 30.0*f*f*(f*(f - 2.0) + 1.0);
#else
	// cubic interpolation
	float2 u = f * f*(3.0 - 2.0*f);
	float2 du = 6.0*f*(1.0 - f);
#endif    

	float va = hash(i + float2(0.0, 0.0));
	float vb = hash(i + float2(1.0, 0.0));
	float vc = hash(i + float2(0.0, 1.0));
	float vd = hash(i + float2(1.0, 1.0));

	float k0 = va;
	float k1 = vb - va;
	float k2 = vc - va;
	float k4 = va - vb - vc + vd;

	return float3(va + (vb - va)*u.x + (vc - va)*u.y + (va - vb - vc + vd)*u.x*u.y, // value
		du*(u.yx*(va - vb - vc + vd) + float2(vb, vc) - va));     // derivative                
}

float noise(in float2 p)
{
	float2 i = floor(p);
	float2 f = frac(p);

	float2 u = f * f*(3.0 - 2.0*f);

	return lerp(
		lerp(hash(i + float2(0.0, 0.0)), hash(i + float2(1.0, 0.0)), u.x),
		lerp(hash(i + float2(0.0, 1.0)), hash(i + float2(1.0, 1.0)), u.x),
		u.y);
}

float pnoise(float2 p)
{
	float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
	float f = 0.5000*noise(p);
	p = mul(p, m);
	f += 0.2500*noise(p);
	p = mul(p, m);
	f += 0.1250*noise(p);
	p = mul(p, m);
	f += 0.0625*noise(p);
	p = mul(p, m);
	return f;
}