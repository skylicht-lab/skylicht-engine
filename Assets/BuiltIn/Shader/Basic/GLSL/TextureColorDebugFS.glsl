precision mediump float;

uniform sampler2D uTexDiffuse;
uniform vec2 uDebug;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

void main(void)
{
	vec4 color = texture(uTexDiffuse, varTexCoord0.xy);

	if (uDebug.x == 1.0)
	{
		color = vec4(color.r, color.r, color.r, color.a);
	}
	else if (uDebug.x == 2.0)
	{
		color = vec4(color.g, color.g, color.g, color.a);
	}
	else if (uDebug.x == 3.0)
	{
		color = vec4(color.b, color.b, color.b, color.a);
	}
	else if (uDebug.x == 4.0)
	{
		color = vec4(color.a, color.a, color.a, 1.0);
	}
	
	if (uDebug.y == 1.0)
	{
		color.a = 1.0;
	}
	
	FragColor = color * varColor;
}