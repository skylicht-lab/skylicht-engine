precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexAlpha;

uniform vec4 uColorSaturation;

in vec2 varTexCoord0;
out vec4 FragColor;

void main(void)
{
	vec4 alphaColor = texture(uTexAlpha, varTexCoord0.xy);

	if (alphaColor.r < 0.5)
		discard;
	
	vec3 result = texture(uTexDiffuse, varTexCoord0.xy).rgb;
	FragColor = vec4(result, 1.0);
}