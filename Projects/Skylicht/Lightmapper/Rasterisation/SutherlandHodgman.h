#pragma once

void SutherlandHodgman(
	core::vector2df *subjectPolygon,
	const int &subjectPolygonSize,
	core::vector2df *clipPolygon,
	const int &clipPolygonSize,
	core::vector2df *newPolygon,
	int &newPolygonSize);
