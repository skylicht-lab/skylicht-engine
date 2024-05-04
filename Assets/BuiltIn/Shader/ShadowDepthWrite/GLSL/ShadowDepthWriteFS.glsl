precision highp float;

in vec4 varPos;
in vec4 varWorldPos;

out vec4 FragColor;

void main(void)
{
	float depth = varPos.z / varPos.w;
	depth = depth * 0.5 + 0.5;

	FragColor = vec4(depth, 0.0, 0.0, 0.0);
}