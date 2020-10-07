// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// https://www.shadertoy.com/view/4dXBRH

float hash(in vec2 p)  // replace this by something better
{
	p = 17.0*fract(p*0.3183099 + vec2(0.71, 0.113));
	return -1.0 + 2.0*fract(p.x*p.y*(p.x + p.y));
}

// return value noise (in x) and its derivatives (in yz)
vec3 noised(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

#if 1
	// quintic interpolation
	vec2 u = f * f*f*(f*(f*6.0 - 15.0) + 10.0);
	vec2 du = 30.0*f*f*(f*(f - 2.0) + 1.0);
#else
	// cubic interpolation
	vec2 u = f * f*(3.0 - 2.0*f);
	vec2 du = 6.0*f*(1.0 - f);
#endif    

	float va = hash(i + vec2(0.0, 0.0));
	float vb = hash(i + vec2(1.0, 0.0));
	float vc = hash(i + vec2(0.0, 1.0));
	float vd = hash(i + vec2(1.0, 1.0));

	float k0 = va;
	float k1 = vb - va;
	float k2 = vc - va;
	float k4 = va - vb - vc + vd;

	return vec3(va + (vb - va)*u.x + (vc - va)*u.y + (va - vb - vc + vd)*u.x*u.y, // value
		du*(u.yx*(va - vb - vc + vd) + vec2(vb, vc) - va));     // derivative                
}

float noise(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = f * f*(3.0 - 2.0*f);

	return mix(
		mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
		mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x),
		u.y
	);
}

float pnoise(vec2 p)
{
	mat2 m = mat2(1.6, 1.2, -1.2, 1.6);
	float f = 0.5000*noise(p); p = m * p;
	f += 0.2500*noise(p); p = m * p;
	f += 0.1250*noise(p); p = m * p;
	f += 0.0625*noise(p); p = m * p;
	return f;
}