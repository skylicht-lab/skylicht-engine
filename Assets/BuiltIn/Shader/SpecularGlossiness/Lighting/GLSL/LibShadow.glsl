float shadow(const vec4 shadowCoord[2], const float shadowDistance[2], const float farDistance)
{	
	int id = 0;
	float visible = 1.0;
	float bias = 0.0002;
	float depth = 0.0;
	float sampledDistance = 0.0;
	
	if (farDistance > shadowDistance[0])
		id = 1;
	if (farDistance > shadowDistance[1])
		return 1.0;
	
	depth = shadowCoord[id].z;
	sampledDistance = texture(uShadowMap, vec3(shadowCoord[id].xy, id)).r;
	
	if (depth - bias > sampledDistance)
        visible = 0.1f;
		
	return visible;
}