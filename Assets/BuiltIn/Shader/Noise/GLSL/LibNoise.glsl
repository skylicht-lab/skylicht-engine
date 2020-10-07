// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// https://www.shadertoy.com/view/4sfGzS

#define USE_PROCEDURAL

#ifdef USE_PROCEDURAL
float hash(vec3 p)  // replace this by something better
{
	p = fract(p*0.3183099 + .1);
	p *= 17.0;
	return fract(p.x*p.y*p.z*(p.x + p.y + p.z));
}

float noise(in vec3 x)
{
	vec3 i = floor(x);
	vec3 f = fract(x);
	f = f * f*(3.0 - 2.0*f);

	return mix(
		mix(
			mix(hash(i + vec3(0, 0, 0)), hash(i + vec3(1, 0, 0)), f.x),
			mix(hash(i + vec3(0, 1, 0)), hash(i + vec3(1, 1, 0)), f.x),
			f.y
		),
		mix(
			mix(hash(i + vec3(0, 0, 1)), hash(i + vec3(1, 0, 1)), f.x),
			mix(hash(i + vec3(0, 1, 1)), hash(i + vec3(1, 1, 1)), f.x),
			f.y
		),
		f.z);
}
#else
float noise(in vec3 x)
{
	vec3 i = floor(x);
	vec3 f = fract(x);
	f = f * f*(3.0 - 2.0*f);
	vec2 uv = (i.xy + vec2(37.0, 17.0)*i.z) + f.xy;
	vec2 rg = textureLod(uNoiseTexture, (uv + 0.5) / 256.0, 0.0).yx;
	return mix(rg.x, rg.y, f.z);
}
#endif

const mat3 m = mat3(0.00, 0.80, 0.60,
	-0.80, 0.36, -0.48,
	-0.60, -0.48, 0.64);

float pnoise(vec3 q)
{
	float f = 0.5000*noise(q); q = m * q*2.01;
	f += 0.2500*noise(q); q = m * q*2.02;
	f += 0.1250*noise(q); q = m * q*2.03;
	f += 0.0625*noise(q); q = m * q*2.01;

	return -1.0 + f * 2.0;
}