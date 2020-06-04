precision highp float;

uniform samplerCube uTexReflect;

in vec4 vColor;
in vec3 vWorldNormal;

out vec4 FragColor;

void main(void)
{	
	FragColor = vColor * textureLod(uTexReflect, vWorldNormal, 0);
}