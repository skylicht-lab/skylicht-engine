float centerX = 0.5 / uTransformTextureSize.x;
float centerY = 0.5 / uTransformTextureSize.y;

float nextPixelX = 1.0 / uTransformTextureSize.x;
float nextPixelY = 1.0 / uTransformTextureSize.y;

mat4 getTransformFromTexture(vec2 p)
{
	vec2 uv = vec2(
		p.x * nextPixelX * 4.0 + centerX, 
		p.y * nextPixelY + centerY
	);
	
	vec4 c1 = textureLod(uTransformTexture, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	vec4 c2 = textureLod(uTransformTexture, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	vec4 c3 = textureLod(uTransformTexture, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	vec4 c4 = textureLod(uTransformTexture, uv, 0.0);
	uv.x = uv.x + nextPixelX;
	
	return mat4(c1, c2, c3, c4);
}