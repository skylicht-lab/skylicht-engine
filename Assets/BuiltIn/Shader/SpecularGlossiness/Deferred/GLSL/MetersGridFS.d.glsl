precision highp float;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in float vTangentW;

#ifdef INSTANCING
in vec4 vColor;
in vec2 vSpecGloss;
#else
uniform vec4 uColor;
uniform vec2 uSpecGloss;

#ifdef CUSTOM_COLOR
uniform vec4 uLightColor;
uniform vec4 uDarkColor;
uniform vec4 uBorderColor;
#endif

#endif

#ifndef CUSTOM_COLOR
const vec4 uLightColor = vec4(0.3, 0.3, 0.307843, 1.0);
const vec4 uDarkColor = vec4(0.156863, 0.156863, 0.160784, 1.0);
const vec4 uBorderColor = vec4(0.8, 0.8, 0.8, 1.0);
#endif

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

const float gridSize = 4.0;
float border = gridSize / 200.0;

float gridBorder(vec3 uv3, float borderWidth, float aa)
{
	vec2 uv = uv3.xz + uv3.xy + uv3.yz;
	
	vec2 dx = dFdx(uv);
	vec2 dy = dFdy(uv);
	
	vec4 uvDDXY = vec4(dx, dy);
	vec2 uvDeriv = vec2(length(uvDDXY.xz), length(uvDDXY.yw));
	
	vec2 targetWidth = vec2(borderWidth, borderWidth);
	
	vec2 lineAA = uvDeriv * 1.5 * aa;
	vec2 gridUV = vec2(1.0, 1.0) - abs(fract(uv3.xz) * 2.0 - vec2(1.0, 1.0));
	
	vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(1.0, 1.0));
	vec2 gridBase  = smoothstep(drawWidth  + lineAA, drawWidth - lineAA, gridUV);
	
	vec2 greyScale = clamp(targetWidth / drawWidth, 0.0, 1.0);	
	gridBase *= greyScale;
	
	return mix(gridBase.x, 1.0, gridBase.y);
}

void main(void)
{
	vec3 pos = vWorldPosition.xyz;
	
	vec3 uv = pos / gridSize;
	float grid = gridBorder(uv, border, 1.0);
	
	pos.x = pos.x + gridSize / 2.0;
	pos.z = pos.z + gridSize / 2.0;
	uv = pos / (gridSize * 2.0);
	float checkerBoard = gridBorder(uv, 0.5, 1.5);
	
	vec3 col = mix(uLightColor.rgb, uDarkColor.rgb, vec3(checkerBoard, checkerBoard, checkerBoard));
	col = mix(col, uBorderColor.rgb, vec3(grid, grid, grid));
	
#ifdef INSTANCING
	Diffuse = vColor * vec4(col.r, col.g, col.b, 1.0);
	SG = vec4(vSpecGloss.x, vSpecGloss.y, 0.0, 1.0);
#else
	Diffuse = uColor * vec4(col.r, col.g, col.b, 1.0);
	SG = vec4(uSpecGloss.x, uSpecGloss.y, 0.0, 1.0);
#endif

	Position = vWorldPosition;
	Normal = vec4(vWorldNormal, 1.0);
}
