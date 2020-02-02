precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;

uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uAmbientLightColor;
uniform vec4 uLightColor;

in vec2	varTexCoord0;

out vec4 FragColor;

#include "LibSG.glsl"

void main(void)
{	
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
		
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir 	= normalize(v);
		
	vec3 color = SG(
		albedo, 
		data.r, 
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uAmbientLightColor.rgb,
		uLightColor.rgb);
	
	FragColor = vec4(color, 1.0);
}