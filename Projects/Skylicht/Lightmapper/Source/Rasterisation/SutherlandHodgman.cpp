#include "pch.h"
#include "SutherlandHodgman.h"

// https://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping#C.2B.2B

const int N = 99;

// check if a point is on the LEFT side of an edge
bool inside(core::vector2df p, core::vector2df p1, core::vector2df p2)
{
	return (p2.Y - p1.Y) * p.X + (p1.X - p2.X) * p.Y + (p2.X * p1.Y - p1.X * p2.Y) < 0;
}

// calculate intersection point
core::vector2df intersection(core::vector2df cp1, core::vector2df cp2, core::vector2df s, core::vector2df e)
{
	core::vector2df dc = { cp1.X - cp2.X, cp1.Y - cp2.Y };
	core::vector2df dp = { s.X - e.X, s.Y - e.Y };

	float n1 = cp1.X * cp2.Y - cp1.Y * cp2.X;
	float n2 = s.X * e.Y - s.Y * e.X;
	float n3 = 1.0f / (dc.X * dp.Y - dc.Y * dp.X);

	return { (n1 * dp.X - n2 * dc.X) * n3, (n1 * dp.Y - n2 * dc.Y) * n3 };
}

// Sutherland-Hodgman clipping
void SutherlandHodgman(core::vector2df *subjectPolygon, const int &subjectPolygonSize, core::vector2df *clipPolygon, const int &clipPolygonSize, core::vector2df *newPolygon, int &newPolygonSize)
{
	core::vector2df cp1, cp2, s, e, inputPolygon[N];

	// copy subject polygon to new polygon and set its size
	for (int i = 0; i < subjectPolygonSize; i++)
		newPolygon[i] = subjectPolygon[i];

	newPolygonSize = subjectPolygonSize;

	for (int j = 0; j < clipPolygonSize; j++)
	{
		// copy new polygon to input polygon & set counter to 0
		for (int k = 0; k < newPolygonSize; k++) { inputPolygon[k] = newPolygon[k]; }
		int counter = 0;

		// get clipping polygon edge
		cp1 = clipPolygon[j];
		cp2 = clipPolygon[(j + 1) % clipPolygonSize];

		for (int i = 0; i < newPolygonSize; i++)
		{
			// get subject polygon edge
			s = inputPolygon[i];
			e = inputPolygon[(i + 1) % newPolygonSize];

			// Case 1: Both vertices are inside:
			// Only the second vertex is added to the output list
			if (inside(s, cp1, cp2) && inside(e, cp1, cp2))
				newPolygon[counter++] = e;

			// Case 2: First vertex is outside while second one is inside:
			// Both the point of intersection of the edge with the clip boundary
			// and the second vertex are added to the output list
			else if (!inside(s, cp1, cp2) && inside(e, cp1, cp2))
			{
				newPolygon[counter++] = intersection(cp1, cp2, s, e);
				newPolygon[counter++] = e;
			}

			// Case 3: First vertex is inside while second one is outside:
			// Only the point of intersection of the edge with the clip boundary
			// is added to the output list
			else if (inside(s, cp1, cp2) && !inside(e, cp1, cp2))
				newPolygon[counter++] = intersection(cp1, cp2, s, e);

			// Case 4: Both vertices are outside
			else if (!inside(s, cp1, cp2) && !inside(e, cp1, cp2))
			{
				// No vertices are added to the output list
			}
		}

		// set new polygon size
		newPolygonSize = counter;
	}
}