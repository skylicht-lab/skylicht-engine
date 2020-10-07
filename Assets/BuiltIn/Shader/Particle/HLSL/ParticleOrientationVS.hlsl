struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float3 particlePos: TEXCOORD1;
	float4 particleColor: TEXCOORD2;
	float2 particleUVScale: TEXCOORD3;
	float2 particleUVOffset: TEXCOORD4;
	float3 particleSize: TEXCOORD5;
	float3 particleRotation: TEXCOORD6;
	float3 particleVelocity: TEXCOORD7;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float3 uOrientationUp;
	float3 uOrientationNormal;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	// rotate
	float cosA = cos(input.particleRotation.z);
	float sinA = sin(input.particleRotation.z);
	float oneMinusCosA = 1.0 - cosA;
	
	float upX = (uOrientationNormal.x * uOrientationNormal.x + (1.0f - uOrientationNormal.x * uOrientationNormal.x) * cosA) * uOrientationUp.x
		+ (uOrientationNormal.x * uOrientationNormal.y * oneMinusCosA - uOrientationNormal.z * sinA) * uOrientationUp.y
		+ (uOrientationNormal.x * uOrientationNormal.z * oneMinusCosA + uOrientationNormal.y * sinA) * uOrientationUp.z;
	
	float upY = (uOrientationNormal.x * uOrientationNormal.y * oneMinusCosA + uOrientationNormal.z * sinA) * uOrientationUp.x
		+ (uOrientationNormal.y * uOrientationNormal.y + (1.0f - uOrientationNormal.y * uOrientationNormal.y) * cosA) * uOrientationUp.y
		+ (uOrientationNormal.y * uOrientationNormal.z * oneMinusCosA - uOrientationNormal.x * sinA) * uOrientationUp.z;

	float upZ = (uOrientationNormal.x * uOrientationNormal.z * oneMinusCosA - uOrientationNormal.y * sinA) * uOrientationUp.x
		+ (uOrientationNormal.y * uOrientationNormal.z * oneMinusCosA + uOrientationNormal.x * sinA) * uOrientationUp.y
		+ (uOrientationNormal.z * uOrientationNormal.z + (1.0f - uOrientationNormal.z * uOrientationNormal.z) * cosA) * uOrientationUp.z;
	
	// billboard position
	float3 up = float3(upX, upY, upZ);
	up = normalize(up);
	
	float3 side = cross(up, uOrientationNormal);
	side = normalize(side);
	
	side = side * 0.5 * input.particleSize.x;
	up = up * 0.5 * input.particleSize.y;
	
	float3 position = input.particlePos + input.pos.x * side + input.pos.y * up;
	
	output.pos = mul(float4(position, 1.0), uMvpMatrix);	
	output.color = input.particleColor;
	output.tex0 = input.tex0 * input.particleUVScale + input.particleUVOffset;
	
	return output;
}