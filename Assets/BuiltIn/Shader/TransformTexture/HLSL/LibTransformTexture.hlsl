static const float centerX = 0.5 / uTransformTextureSize.x;
static const float centerY = 0.5 / uTransformTextureSize.y;

static const float nextPixelX = 1.0 / uTransformTextureSize.x;
static const float nextPixelY = 1.0 / uTransformTextureSize.y;

float4x4 getTransformFromTexture(float2 p)
{
	float2 uv = float2(
		p.x * nextPixelX * 4.0 + centerX, 
		p.y * nextPixelY + centerY
	);
	
	float4 c1 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	float4 c2 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	float4 c3 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	float4 c4 = uTransformTexture.SampleLevel(uTransformTextureSampler, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	
	return float4x4(c1, c2, c3, c4);
}