#ifdef INSTANCING

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;

#ifdef UV2
layout(location = 4) in vec2 inTexCoord1;
layout(location = 5) in vec3 inLightmap;
layout(location = 6) in vec4 uUVScale;
layout(location = 7) in vec4 uUVScale1;
layout(location = 8) in vec4 uColor;
layout(location = 9) in mat4 uWorldMatrix;
#else
layout(location = 4) in vec4 uUVScale;
layout(location = 5) in vec4 uColor;
layout(location = 6) in mat4 uWorldMatrix;
#endif

#else
	
in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#ifdef UV2
in vec2 inTexCoord1;
#endif

#endif

#ifdef INSTANCING
uniform mat4 uVPMatrix;
#else
uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;
#ifdef UV2
uniform vec4 uUVScale1;
#endif
#endif

#ifdef RIM_LIGHT
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
#endif

out vec2 varTexCoord0;

#ifdef UV2
out vec2 varTexCoord1;
#endif

out vec4 varUVScale;
out vec4 varUVScale1;

#ifdef RIM_LIGHT
out vec3 varWorldNormal;
out vec3 varWorldViewDir;
#endif

out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0;
	varUVScale = uUVScale;

#ifdef UV2
	varUVScale1 = uUVScale1;
	varTexCoord1 = inTexCoord1;
#endif

#ifdef INSTANCING
	varColor = uColor;
#else
	varColor = inColor / 255.0;
#endif

#if defined(INSTANCING) || defined(RIM_LIGHT)
	vec4 worldPos = uWorldMatrix * inPosition;
#endif

#ifdef RIM_LIGHT
	varWorldViewDir = normalize((uCameraPosition - worldPos).xyz);
	
	vec4 worldNormal = uWorldMatrix * vec4(inNormal, 0.0);
	varWorldNormal = normalize(worldNormal.xyz);
#endif

#ifdef INSTANCING
	gl_Position = uVPMatrix * worldPos;
#else
	gl_Position = uMvpMatrix * inPosition;
#endif
}