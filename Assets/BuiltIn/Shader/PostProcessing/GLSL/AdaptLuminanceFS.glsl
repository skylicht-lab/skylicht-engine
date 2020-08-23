precision mediump float;

uniform sampler2D uTexLuminance;
uniform sampler2D uTexLastLuminance;

uniform vec2 uTimeStep;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

void main(void)
{
	float lum = texture(uTexLuminance, varTexCoord0).x;
	float lastLum = texture(uTexLastLuminance, varTexCoord0).x;
	
	float adaptLum = lastLum + (lum - lastLum) * uTimeStep.x * 0.001;
	
	FragColor = vec4(adaptLum, 0, 0, 0);	 
}