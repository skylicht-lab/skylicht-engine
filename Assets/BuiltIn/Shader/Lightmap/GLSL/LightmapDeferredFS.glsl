precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2DArray uTexLightmap;

uniform float uLightmapIndex;

in vec3 varTexCoord0;

layout(location = 0) out vec4 Indirect;
layout(location = 1) out vec4 Directional;

void main(void)
{
	Indirect = texture(uTexLightmap, varTexCoord0) * 3.0;
	
	vec3 uv = varTexCoord0;
	uv.z += uLightmapIndex;
	vec4 lightColor = texture(uTexLightmap, uv);
	Directional = vec4(lightColor.rgb * 3.0, lightColor.a);
}