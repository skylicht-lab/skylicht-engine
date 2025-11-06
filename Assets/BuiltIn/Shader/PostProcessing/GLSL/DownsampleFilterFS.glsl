precision mediump float;

uniform sampler2D uSourceTex;

uniform vec2 uTexelSize;

in vec2	varTexCoord0;

out vec4 FragColor;

const float weights[3] = float[3](0.25, 0.5, 0.25);
// const float weights[9] = float[9](0.05, 0.09, 0.12, 0.15, 0.16, 0.15, 0.12, 0.09, 0.05);

void main(void)
{
	vec3 sum = vec3(0.0, 0.0, 0.0);	
	
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, -1.0 * uTexelSize.y), 0.0).rgb * weights[0];
	sum += textureLod(uSourceTex, varTexCoord0, 0.0).rgb * weights[1];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, 1.0 * uTexelSize.y), 0.0).rgb * weights[2];

	/*
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, -4.0 * uTexelSize.y), 0.0).rgb * weights[0];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, -3.0 * uTexelSize.y), 0.0).rgb * weights[1];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, -2.0 * uTexelSize.y), 0.0).rgb * weights[2];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, -1.0 * uTexelSize.y), 0.0).rgb * weights[3];
	sum += textureLod(uSourceTex, varTexCoord0, 0.0).rgb * weights[4];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, 1.0 * uTexelSize.y), 0.0).rgb * weights[5];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, 2.0 * uTexelSize.y), 0.0).rgb * weights[6];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, 3.0 * uTexelSize.y), 0.0).rgb * weights[7];
	sum += textureLod(uSourceTex, varTexCoord0 + vec2(0.0, 4.0 * uTexelSize.y), 0.0).rgb * weights[8];
	*/
	
	FragColor = vec4(sum, 1.0);
}