// Number of thread
#define MAX_NUM_THREAD 128

// MAX_NUM_THREAD * 6 faces
#define TANGENT_COUNT 768

// NUM FACE
#define NUM_FACE 6

// Size each group thread
#define RT_SIZE 16

// Uniform Constants
cbuffer cbConstants
{
	float4x4 uToTangentSpace[TANGENT_COUNT];
	float2 uPixelOffset;
	float2 uFaceSize;
}

// uRadianceMap texture
Texture2D uRadianceMap : register(t0);

// Output result
RWBuffer<float4> OutputBuffer : register(u0);

// Share group thread data
groupshared float3 ResultSH[RT_SIZE * RT_SIZE][9];

// SH compute function
void ProjectOntoSH(in float3 n, in float3 color, out float3 sh[9])
{
	// Band 0
	sh[0] = 0.282095f * color;

	// Band 1
	sh[1] = 0.488603f * n.y * color;
	sh[2] = 0.488603f * n.z * color;
	sh[3] = 0.488603f * n.x * color;

	// Band 2
	sh[4] = 1.092548f * n.x * n.y * color;
	sh[5] = 1.092548f * n.y * n.z * color;
	sh[6] = 0.315392f * (3.0f * n.z * n.z - 1.0f) * color;
	sh[7] = 1.092548f * n.x * n.z * color;
	sh[8] = 0.546274f * (n.x * n.x - n.y * n.y) * color;
}

// Params:
// groupID.x [0 -> MAX_NUM_THREAD]
// groupID.y [0 -> NUM_FACE]

// groupThreadID.x [0 -> RT_SIZE]
// groupThreadID.y [0 -> RT_SIZE]

// https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-attributes-numthreads
[numthreads(RT_SIZE, RT_SIZE, 1)]
void main(
	uint3 groupID : SV_GroupID,
	uint3 groupThreadID : SV_GroupThreadID,
	uint3 dispatchThreadID : SV_DispatchThreadID,
	uint groupIndex : SV_GroupIndex)
{
	// begin run thread (see dispatch function)
	uint threadID = groupID.x;
	uint faceID = groupID.y;

	uint pixelX = groupThreadID.x;
	uint pixelY = groupThreadID.y;

	// need init variable
	if (uPixelOffset.x == 0 && uPixelOffset.y == 0 && pixelX == 0 && pixelY == 0)
	{
		uint result = 0;

		// Calc offset id
		uint id = (threadID * NUM_FACE + faceID) * 9;

		float4 zero = float4(0.0, 0.0, 0.0f, 0.0);

		// Write result
		OutputBuffer[id + 0] = zero;
		OutputBuffer[id + 1] = zero;
		OutputBuffer[id + 2] = zero;
		OutputBuffer[id + 3] = zero;
		OutputBuffer[id + 4] = zero;
		OutputBuffer[id + 5] = zero;
		OutputBuffer[id + 6] = zero;
		OutputBuffer[id + 7] = zero;
		OutputBuffer[id + 8] = zero;
	}

	GroupMemoryBarrierWithGroupSync();

	const int3 pixelLocation = int3(pixelX + (int)uPixelOffset.x, pixelY + (int)uPixelOffset.y, 0.0);

	const int3 location = int3(
		pixelLocation.x + faceID * uFaceSize.x,
		pixelLocation.y + threadID * uFaceSize.y,
		0.0);

	// Gather RGB from the texels
	float3 radiance = uRadianceMap.Load(location).xyz;

	// Calculate the location in [-1, 1] texture space
	float u = (pixelLocation.x / float(uFaceSize.x)) * 2.0f - 1.0f;
	float v = -((pixelLocation.y / float(uFaceSize.y)) * 2.0f - 1.0f);

	// Calculate weight
	float temp = 1.0f + u * u + v * v;
	float weight = 4.0f / (sqrt(temp) * temp);
	radiance *= weight;

	// Extract direction from texel u,v
	float3 dirVS = float3(u, v, 1.0f);
	float3 dirTS = mul(dirVS, (float3x3)uToTangentSpace[threadID * NUM_FACE + faceID]);
	dirTS = normalize(dirTS);

	// Project onto SH
	float3 sh[9];
	ProjectOntoSH(dirTS, radiance, sh);

	// SH add
	uint pixel = pixelY * RT_SIZE + pixelX;

	ResultSH[pixel][0] = sh[0];
	ResultSH[pixel][1] = sh[1];
	ResultSH[pixel][2] = sh[2];
	ResultSH[pixel][3] = sh[3];
	ResultSH[pixel][4] = sh[4];
	ResultSH[pixel][5] = sh[5];
	ResultSH[pixel][6] = sh[6];
	ResultSH[pixel][7] = sh[7];
	ResultSH[pixel][8] = sh[8];

	GroupMemoryBarrierWithGroupSync();

	// Sum total SH[RT_SIZE * RT_SIZE] by GPU MT store at [0]
	// ref: https://developer.nvidia.com/gpugems/gpugems3/part-vi-gpu-computing/chapter-39-parallel-prefix-sum-scan-cuda
	uint totalSize = RT_SIZE * RT_SIZE;
	// Example: totalSize = 8
	//      [0] [1] [2] [3] [4] [5] [6] [7]
	//       .  .    .  .    .  .    .  .
	//       . .     . .     . .     . .
	// s=1  [0]     [2]     [4]     [6]
	//       .     .         .     .
	//       .   .           .   .
	// s=2  [0]             [4]
	//       .            .
	//       .         .
	//       .      .
	//       .   .
	// s=4  [0]
	for (uint s = 1; s < totalSize; s *= 2)
	{
		if (pixel % (s * 2) == 0)
		{
			ResultSH[pixel][0] += ResultSH[pixel + s][0];
			ResultSH[pixel][1] += ResultSH[pixel + s][1];
			ResultSH[pixel][2] += ResultSH[pixel + s][2];
			ResultSH[pixel][3] += ResultSH[pixel + s][3];
			ResultSH[pixel][4] += ResultSH[pixel + s][4];
			ResultSH[pixel][5] += ResultSH[pixel + s][5];
			ResultSH[pixel][6] += ResultSH[pixel + s][6];
			ResultSH[pixel][7] += ResultSH[pixel + s][7];
			ResultSH[pixel][8] += ResultSH[pixel + s][8];
		}
		GroupMemoryBarrierWithGroupSync();
	}
	
	// Write result on first group thread
	if (pixel == 0)
	{
		uint result = 0;

		// Calc offset id
		uint id = (threadID * NUM_FACE + faceID) * 9;

		// Write result
		OutputBuffer[id + 0] += float4(ResultSH[0][0], 0.0);
		OutputBuffer[id + 1] += float4(ResultSH[0][1], 0.0);
		OutputBuffer[id + 2] += float4(ResultSH[0][2], 0.0);
		OutputBuffer[id + 3] += float4(ResultSH[0][3], 0.0);
		OutputBuffer[id + 4] += float4(ResultSH[0][4], 0.0);
		OutputBuffer[id + 5] += float4(ResultSH[0][5], 0.0);
		OutputBuffer[id + 6] += float4(ResultSH[0][6], 0.0);
		OutputBuffer[id + 7] += float4(ResultSH[0][7], 0.0);
		OutputBuffer[id + 8] += float4(ResultSH[0][8], 0.0);
	}
}