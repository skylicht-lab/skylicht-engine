precision mediump float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

uniform vec4 uColorIntensity;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec4 texColor = texture(uTexture, varTexCoord0.xy);
	vec3 color = mix(vec3(0.0, 0.0, 0.0), texColor.rgb, texColor.a);
	vec3 result = sRGB(color * varColor.rgb * varColor.a * uColorIntensity.rgb);
	FragColor = vec4(result, 1.0);
}