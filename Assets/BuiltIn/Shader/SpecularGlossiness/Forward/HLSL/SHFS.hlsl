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

struct SH9Color
{
	float3 c[9];
};

struct SH4
{
	float c[4];
};

struct H6Color
{
	float3 c[6];
};

// References:
// https://github.com/TheRealMJP/BakingLab/blob/master/SampleFramework11/v1.02/Shaders/SH.hlsl
//-------------------------------------------------------------------------------------------------

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

	return (sh.c[0] * uSHConst[0].xyz +
		sh.c[1] * uSHConst[1].xyz +
		sh.c[2] * uSHConst[2].xyz +
		sh.c[3] * uSHConst[3].xyz +
		sh.c[4] * uSHConst[4].xyz +
		sh.c[5] * uSHConst[5].xyz +
		sh.c[6] * uSHConst[6].xyz +
		sh.c[7] * uSHConst[7].xyz +
		sh.c[8] * uSHConst[8].xyz);
}

//-------------------------------------------------------------------------------------------------
// Evaluates the irradiance from a set of SH4 coeffecients using the non-linear fit from
// the paper by Graham Hazel from Geomerics.
// https://grahamhazel.com/blog/2017/12/22/converting-sh-radiance-to-irradiance/
//-------------------------------------------------------------------------------------------------
float EvalSH4IrradianceGeomerics(in float3 dir, in SH4 sh)
{
	float R0 = sh.c[0];

	float3 R1 = 0.5f * float3(sh.c[3], sh.c[1], sh.c[2]);
	float lenR1 = length(R1);

	float q = 0.5f * (1.0f + dot(R1 / lenR1, dir));

	float p = 1.0f + 2.0f * lenR1 / R0;
	float a = (1.0f - lenR1 / R0) / (1.0f + lenR1 / R0);

	return R0 * (a + (1.0f - a) * (p + 1.0f) * pow(abs(q), p));
}

float3 EvalSH4IrradianceGeomerics(in float3 dir)
{
	SH4 shr = { uSHConst[0].x, uSHConst[1].x, uSHConst[2].x, uSHConst[3].x };
	SH4 shg = { uSHConst[0].y, uSHConst[1].y, uSHConst[2].y, uSHConst[3].y };
	SH4 shb = { uSHConst[0].z, uSHConst[1].z, uSHConst[2].z, uSHConst[3].z };

	return float3(EvalSH4IrradianceGeomerics(dir, shr),
		EvalSH4IrradianceGeomerics(dir, shg),
		EvalSH4IrradianceGeomerics(dir, shb));
}

float4 main(PS_INPUT input) : SV_TARGET
{
	// SH4
	// float3 ambient = EvalSH4IrradianceGeomerics(input.worldNormal);

	// SH9
	float3 ambient = EvalSH9Irradiance(input.worldNormal);

	// Test boost indirect
	// float3 shBrightness  = uSHConst[0];	
	// float l = shBrightness.r * 0.21 + shBrightness.g * 0.72 + shBrightness.b * 0.07;
	// float interplateMultipler = 1.0 + 1.5 * (1.0 - l);
	// ambient *= interplateMultipler;

	float4 result = input.color * float4(ambient, 1.0) / 3.14;
	return result;
}