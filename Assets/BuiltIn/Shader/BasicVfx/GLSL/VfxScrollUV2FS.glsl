// File Generated by Assets/BuildShader.py - source: [BasicVfxFS.d.glsl : UV2,SCROLL_UV2]
precision mediump float;
uniform sampler2D uTexDiffuse1;
uniform sampler2D uTexDiffuse2;
in vec2 varTexCoord0;
in vec2 varTexCoord1;
in vec4 varUVScale;
in vec4 varUVScale1;
in vec4 varColor;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform vec4 uColorIntensity;
uniform vec4 uTime;
out vec4 FragColor;
void main(void)
{
	vec2 tex0 = varTexCoord0 * varUVScale.xy + varUVScale.zw;
	vec2 tex1 = varTexCoord1 * varUVScale1.xy + varUVScale1.zw;
	vec2 uvOffset = vec2(uTime.x, 0.0) / 4.0;
	vec4 color1 = texture(uTexDiffuse1, tex0);
	vec4 color2 = texture(uTexDiffuse2, tex1 + uvOffset);
	vec4 result = (color1 * uColor1 + color2 * uColor2) * varColor * uColorIntensity;
	result.a = color1.a * color2.a * uColor1.a * uColor2.a;
	FragColor = result;
}
