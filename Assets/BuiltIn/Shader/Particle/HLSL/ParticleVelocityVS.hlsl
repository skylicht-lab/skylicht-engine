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
	float3 uViewUp;
	float3 uViewLook;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	// rotate
	float cosA = cos(input.particleRotation.z);
	float sinA = sin(input.particleRotation.z);
	float oneMinusCosA = 1.0 - cosA;
	
	float3 velUp = normalize(input.particleVelocity);
	
	float upX = (uViewLook.x * uViewLook.x + (1.0f - uViewLook.x * uViewLook.x) * cosA) * velUp.x
		+ (uViewLook.x * uViewLook.y * oneMinusCosA - uViewLook.z * sinA) * velUp.y
		+ (uViewLook.x * uViewLook.z * oneMinusCosA + uViewLook.y * sinA) * velUp.z;
	
	float upY = (uViewLook.x * uViewLook.y * oneMinusCosA + uViewLook.z * sinA) * velUp.x
		+ (uViewLook.y * uViewLook.y + (1.0f - uViewLook.y * uViewLook.y) * cosA) * velUp.y
		+ (uViewLook.y * uViewLook.z * oneMinusCosA - uViewLook.x * sinA) * velUp.z;

	float upZ = (uViewLook.x * uViewLook.z * oneMinusCosA - uViewLook.y * sinA) * velUp.x
		+ (uViewLook.y * uViewLook.z * oneMinusCosA + uViewLook.x * sinA) * velUp.y
		+ (uViewLook.z * uViewLook.z + (1.0f - uViewLook.z * uViewLook.z) * cosA) * velUp.z;
	
	// billboard position
	float3 up = float3(upX, upY, upZ);
	up = normalize(up);
	
	float3 side = cross(up, uViewLook);
	side = normalize(side);
	
	side = side * 0.5 * input.particleSize.x;
	up = up * 0.5 * input.particleSize.y;
	
	float3 position = input.particlePos + input.pos.x * side + input.pos.y * up;
	
	output.pos = mul(float4(position, 1.0), uMvpMatrix);	
	output.color = input.particleColor;
	output.tex0 = input.tex0 * input.particleUVScale + input.particleUVOffset;
	
	return output;
}