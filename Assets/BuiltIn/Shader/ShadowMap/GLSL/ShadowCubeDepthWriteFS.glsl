precision mediump float;

uniform vec4 uLightPosition;

in vec4 varPos;

out float FragColor;

void main(void)
{
	vec3 lightToVertex = varPos.xyz - uLightPosition.xyz;

	float lightToPixelDistance = length(lightToVertex);

	FragColor = lightToPixelDistance;
}