precision mediump float;

uniform sampler2D uSourceTex;

uniform vec2 uTexelSize;

in vec2	varTexCoord0;

out vec4 FragColor;

vec3 blurFilter(vec2 uv)
{
	vec4 d = vec4(-uTexelSize.x, -uTexelSize.y, uTexelSize.x, uTexelSize.y);

	vec3 s0 = texture(uSourceTex, uv).rgb;
    vec3 s1 = texture(uSourceTex, uv + d.xy).rgb;
    vec3 s2 = texture(uSourceTex, uv + d.zy).rgb;
    vec3 s3 = texture(uSourceTex, uv + d.xw).rgb;
    vec3 s4 = texture(uSourceTex, uv + d.zw).rgb;

    return (s0 + s1 + s2 + s3 + s4) * 0.2;
}

void main(void)
{
	vec3 m = blurFilter(varTexCoord0);	
	FragColor = vec4(m, 1.0);
}