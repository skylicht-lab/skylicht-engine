precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexRamp;

uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uShadowColor;
uniform vec2 uWrapFactor;
uniform vec3 uSpecular;

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;

out vec4 FragColor;

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	
	float NdotL = max((dot(vWorldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x), 0.0);
	
	vec3 rampMap = texture(uTexRamp, vec2(NdotL, NdotL)).rgb;
	
	// Shadows intensity through alpha
	vec3 color = mix(uColor.rgb, uShadowColor.rgb, uColor.a);
	vec3 ramp = mix(color, uColor.rgb, rampMap);
	
	// Specular
	vec3 h = normalize(uLightDirection.xyz + vWorldViewDir);
	float NdotH = max(0, dot(vWorldNormal, h));
	
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	
	FragColor = diffuseMap * uLightColor * vec4(ramp, 1.0) + uLightColor * spec;
}