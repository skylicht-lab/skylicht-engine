struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float4 particlePos: TEXCOORD1;
	float4 particleColor: TEXCOORD2;
	float2 particleUVScale: TEXCOORD3;
	float2 particleUVOffset: TEXCOORD4;
	float4 particleSize: TEXCOORD5;
	float4 particleRotation: TEXCOORD6;
	float4 particleVelocity: TEXCOORD7;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 vertexPos: VERTEX_POSITION;
};

cbuffer cbPerObject
{
	float4x4 uVPMatrix;
	float4x4 uWorld;
};

float4x4 constructMatrix(float3 position, float3 zAxis, float3 scale, float3 rotation) 
{
	// rotate
	float cosYaw = cos(rotation.z);
	float sinYaw = sin(rotation.z);
	float cosPitch = cos(rotation.y);
	float sinPitch = sin(rotation.y);
	float cosRoll = cos(rotation.x);
	float sinRoll = sin(rotation.x);
	
	float4x4 rotationMatrix = float4x4(
		cosYaw * cosPitch, sinYaw * cosPitch, -sinPitch, 0.0,
		cosYaw * sinPitch * sinRoll - sinYaw * cosRoll, sinYaw * sinPitch * sinRoll + cosYaw * cosRoll, cosPitch * sinRoll, 0.0,
		cosYaw * sinPitch * cosRoll + sinYaw * sinRoll, sinYaw * sinPitch * cosRoll - cosYaw * sinRoll, cosPitch * cosRoll, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	
	float3 yTemp = float3(0.0, 1.0, 0.0);
	
	// vel (z) is y up
	if (abs(dot(zAxis, yTemp)) > 0.999) 
		yTemp = float3(0.0, 0.0, 1.0f);
	
	float3 xAxis = normalize(cross(yTemp, zAxis));
	float3 yAxis = normalize(cross(zAxis, xAxis));
	
	xAxis *= scale.x;
	yAxis *= scale.y;
	zAxis *= scale.z;
	
	float4x4 m = float4x4(
		xAxis.x, xAxis.y, xAxis.z, 0.0,
		yAxis.x, yAxis.y, yAxis.z, 0.0,
		zAxis.x, zAxis.y, zAxis.z, 0.0,
		position.x, position.y, position.z, 1.0
	);
	
	return mul(rotationMatrix, m);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float3 vel = normalize(input.particleVelocity.xyz);
	float4x4 world = mul(constructMatrix(input.particlePos.xyz, vel, input.particleSize.xyz, input.particleRotation.xyz), uWorld);
		
	float4 worldPos = mul(input.pos, world);
	float4 worldNormal = mul(float4(input.norm, 0.0), world);
		
	output.pos = mul(worldPos, uVPMatrix);
	output.vertexPos = input.pos.xyz;
	output.worldNormal = normalize(worldNormal.xyz);
	output.color = input.particleColor;
	output.tex0 = input.tex0 * input.particleUVScale + input.particleUVOffset;
	
	return output;
}