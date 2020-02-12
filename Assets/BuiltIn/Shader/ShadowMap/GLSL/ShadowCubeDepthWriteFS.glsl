precision mediump float;

uniform vec4 uLightPosition;

in vec4 varWorldPos;

out float FragColor;

void main(void)
{
	vec3 lightToVertex = varWorldPos.xyz - uLightPosition.xyz;
	
	float lightToPixelDistance = length(lightToVertex);
	
	FragColor = lightToPixelDistance;
}