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

// copilot: Can you find me the code construct the float4x4 from position and scale
float4x4 constructMatrix(float3 position, float3 scale, float3 rotation) 
{
    // Rotation part
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
	
    // Scale & Translation part
    float4x4 translationMatrix = float4x4(
		scale.x, 0.0, 0.0, 0.0,
		0.0, scale.y, 0.0, 0.0,
		0.0, 0.0, scale.z, 0.0,
		position.x, position.y, position.z, 1.0
    );
	
    // Combine them
	return mul(rotationMatrix, translationMatrix);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4x4 world = mul(constructMatrix(input.particlePos.xyz, input.particleSize.xyz, input.particleRotation.xyz), uWorld);
		
	float4 worldPos = mul(input.pos, world);
	float4 worldNormal = mul(float4(input.norm, 0.0), world);
		
	output.pos = mul(worldPos, uVPMatrix);
	output.vertexPos = input.pos.xyz;
	output.worldNormal = normalize(worldNormal.xyz);
	output.color = input.particleColor;
	output.tex0 = input.tex0 * input.particleUVScale + input.particleUVOffset;
	
	return output;
}