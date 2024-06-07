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
#endif

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

const vec3 lightColor = vec3(0.3, 0.3, 0.307843);
const vec3 darkColor = vec3(0.156863, 0.156863, 0.160784);
const vec3 borderColor = vec3(0.8, 0.8, 0.8);

const float gridSize = 4.0;
float h = gridSize / 2.0;
float border = gridSize / 1000.0;

bool borde(float pos) 
{
	float m1_0 = mod(pos, 1.0);
	float m0_5 = mod(pos, 0.5);
	
	return 
		m1_0 < border || 
		(1.0 - m1_0) < border || 
		m0_5 < border || 
		(0.5 - m0_5) < border;
}

void main(void)
{
	vec3 pos = vWorldPosition.xyz;
	
	// This is to avoid the zero incoherence
	if (pos.x <= 0.0) pos.x = abs(pos.x - h);
	if (pos.y <= 0.0) pos.y = abs(pos.y - h);
	if (pos.z <= 0.0) pos.z = abs(pos.z - h);
	pos /= gridSize;
	
	pos.y += float(fract(float(int(pos.x*h))/h));
	pos.z += float(fract(float(int(pos.y*h))/h));
	
	// grid color
	vec3 col = vec3(0.0, 0.0, 0.0);
	if (fract(float(int(pos.z*h))/h) == 0.0) 
		col += lightColor;
	else 
		col += darkColor;
	
	// border color
	if (borde(pos.x)) col = borderColor;
	// if (borde(pos.y)) col = borderColor;
	if (borde(pos.z)) col = borderColor;
	
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
