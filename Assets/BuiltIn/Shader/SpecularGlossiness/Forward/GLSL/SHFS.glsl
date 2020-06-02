precision highp float;

uniform vec4 uSHConst[9];

in vec4 vColor;
in vec3 vWorldNormal;

out vec4 FragColor;

struct SH9
{
	float c[9];
};

// Cosine kernel for SH
const float Pi = 3.141592654f;
const float CosineA0 = Pi;
const float CosineA1 = (2.0f * Pi) / 3.0f;
const float CosineA2 = Pi / 4.0f;

SH9 ProjectOntoSH9(in vec3 n, in float intensity, in float A0, in float A1, in float A2)
{
    SH9 sh;

    // Band 0
    sh.c[0] = 0.282095f * A0 * intensity;

    // Band 1
    sh.c[1] = 0.488603f * n.y * A1 * intensity;
    sh.c[2] = 0.488603f * n.z * A1 * intensity;
    sh.c[3] = 0.488603f * n.x * A1 * intensity;

    // Band 2
    sh.c[4] = 1.092548f * n.x * n.y * A2 * intensity;
    sh.c[5] = 1.092548f * n.y * n.z * A2 * intensity;
    sh.c[6] = 0.315392f * (3.0 * n.z * n.z - 1.0) * A2 * intensity;
    sh.c[7] = 1.092548f * n.x * n.z * A2 * intensity;
    sh.c[8] = 0.546274f * (n.x * n.x - n.y * n.y) * A2 * intensity;

    return sh;
}

vec3 EvalSH9Irradiance(in vec3 dir)
{
	SH9 sh = ProjectOntoSH9(dir, 1.0f, CosineA0, CosineA1, CosineA2);
		
	return (sh.c[0]*uSHConst[0].xyz + 
		sh.c[1]*uSHConst[1].xyz + 
		sh.c[2]*uSHConst[2].xyz + 
		sh.c[3]*uSHConst[3].xyz + 
		sh.c[4]*uSHConst[4].xyz + 
		sh.c[5]*uSHConst[5].xyz + 
		sh.c[6]*uSHConst[6].xyz + 
		sh.c[7]*uSHConst[7].xyz + 
		sh.c[8]*uSHConst[8].xyz);	
	
	/*
	// Optimize SH4
	return uSHConst[0].xyz +
		uSHConst[1].xyz * dir.y +
		uSHConst[2].xyz * dir.z +
		uSHConst[3].xyz * dir.x;
	*/
}

void main(void)
{
	vec3 ambient = EvalSH9Irradiance(vWorldNormal);
	FragColor = vColor * vec4(ambient, 1.0);
}