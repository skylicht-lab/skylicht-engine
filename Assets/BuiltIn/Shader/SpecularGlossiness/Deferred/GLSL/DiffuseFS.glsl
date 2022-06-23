precision highp float;

uniform sampler2D uTexDiffuse;

uniform vec4 uColor;
uniform vec2 uSpecGloss;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in float vTangentW;

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

void main(void)
{
	vec3 baseMap = texture(uTexDiffuse, vTexCoord0.xy).rgb ;

	Diffuse = vec4(baseMap * uColor.rgb, 1.0);
	Position = vWorldPosition;
	Normal = vec4(vWorldNormal, 1.0);
	SG = vec4(uSpecGloss.x, uSpecGloss.y, 1.0, 1.0);
}
