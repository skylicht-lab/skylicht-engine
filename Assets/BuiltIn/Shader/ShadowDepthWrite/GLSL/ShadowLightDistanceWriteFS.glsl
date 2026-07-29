precision mediump float;

uniform vec4 uLightPosition;
uniform vec4 uLightAttenuation;

in vec4 varPos;
in vec4 varWorldPos;

out float FragColor;

void main(void)
{
	vec3 lightToVertex = varWorldPos.xyz - uLightPosition.xyz;

	float lightToPixelDistance = length(lightToVertex) * uLightAttenuation.y;

	FragColor = lightToPixelDistance;
}
