precision highp float;

in vec3 vWorldNormal;
in vec3 vSH0;
in vec3 vSH1;
in vec3 vSH2;
in vec3 vSH3;

out vec4 FragColor;

void main(void)
{
	// quick SH4
	vec3 ambient = vSH0 +
		vSH1 * vWorldNormal.y +
		vSH2 * vWorldNormal.z +
		vSH3 * vWorldNormal.x;
		
	FragColor = vec4(ambient * 3.0, 1.0);
}
