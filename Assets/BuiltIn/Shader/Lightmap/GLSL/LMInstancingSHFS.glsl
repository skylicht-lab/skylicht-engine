precision highp float;

in vec3 vWorldNormal;

in vec3 vD0;
in vec3 vD1;
in vec3 vD2;
in vec3 vD3;

out vec4 FragColor;

void main(void)
{
	// fast SH4
	vec3 ambient = vD0 +
		vD1 * vWorldNormal.y +
		vD2 * vWorldNormal.z +
		vD3 * vWorldNormal.x;
	
	FragColor = vec4(ambient * 0.9, 1.0);	// fix 0.9 for SH4
}
