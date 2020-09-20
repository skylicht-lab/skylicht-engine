in vec4 inPosition;
in vec4 inColor;
in vec3 inNormal;
in vec2 inTexCoord0;
uniform mat4 uMvpMatrix;
uniform mat4 uWorld;
uniform vec3 uNoiseParam;
out vec2 varTexCoord0;
out vec4 varColor;
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
	return f;
}
void main(void)
{
	varTexCoord0 = inTexCoord0;
	varColor = inColor/255.0;
	vec3 worldPos = (uWorld * inPosition).xyz;
	float n = pnoise(worldPos * uNoiseParam.x) * 2.0 - 1.0;
	float weight = clamp(inTexCoord0.y * uNoiseParam.z, 0.0, 1.0);
	vec4 noisePosition = inPosition + vec4(n * inNormal * weight * uNoiseParam.y, 0.0);
	gl_Position = uMvpMatrix * noisePosition;
}
