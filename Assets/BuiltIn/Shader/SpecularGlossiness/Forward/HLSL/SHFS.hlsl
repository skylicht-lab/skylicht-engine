struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 worldNormal: WORLDNORMAL;
};

cbuffer cbPerFrame
{
	float4 uSHConst[9];
};

struct SH9
{
	float c[9];
};

// Cosine kernel for SH
static const float Pi = 3.141592654f;
static const float CosineA0 = Pi;
static const float CosineA1 = (2.0f * Pi) / 3.0f;
static const float CosineA2 = Pi / 4.0f;

SH9 ProjectOntoSH9(in float3 n, in float intensity, in float A0, in float A1, in float A2)
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
    sh.c[6] = 0.315392f * (3.0f * n.z * n.z - 1.0f) * A2 * intensity;
    sh.c[7] = 1.092548f * n.x * n.z * A2 * intensity;
    sh.c[8] = 0.546274f * (n.x * n.x - n.y * n.y) * A2 * intensity;

    return sh;
}

float3 EvalSH9Irradiance(in float3 dir)
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
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 ambient = EvalSH9Irradiance(input.worldNormal);
	float4 result = input.color * float4(ambient, 1.0);
	return result;
}