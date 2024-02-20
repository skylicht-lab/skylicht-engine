precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2DArray uTexLightmap;

uniform float uLightmapIndex;

in vec3 varTexCoord0;

out vec4 FragColor;

void main(void)
{
	vec3 uv = varTexCoord0;
	uv.z += uLightmapIndex;
	vec4 lightColor = texture(uTexLightmap, uv);
	FragColor = vec4(lightColor.rgb * 3.0, lightColor.a);
}