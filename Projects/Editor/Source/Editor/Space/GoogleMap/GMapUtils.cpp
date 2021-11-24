/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "GMapUtils.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GMapConst
		{
			const int MapElementWidth = 256;
			const int MapElementHeight = 256;

			const double EarthRadius = 6378137;

			const double MinLatitude = -85.05112878;
			const double MaxLatitude = 85.05112878;
			const double MinLongitude = -180.0;
			const double MaxLongitude = 180.0;

			const double PI = 3.14159265359;
		}

		double clip(double n, double min, double max)
		{
			double ret = n;

			if (ret < min)
				ret = min;
			else if (ret > max)
				ret = max;

			return ret;
		}

		void getLatLngByPixel(long px, long py, int zoom, double* lat, double* lng)
		{
			double mapSize = (double)(256 << zoom);

			double x = (clip(px, 0, mapSize - 1) / mapSize) - 0.5;
			double y = 0.5 - (clip(py, 0, mapSize - 1) / mapSize);

			*lat = 90.0 - 360.0 * atan(exp(-y * 2 * GMapConst::PI)) / GMapConst::PI;
			*lng = 360.0 * x;
		}

		void getPixelByLatLng(double lat, double lng, int zoom, long* px, long* py)
		{
			lat = clip(lat, GMapConst::MinLatitude, GMapConst::MaxLatitude);
			lng = clip(lng, GMapConst::MinLongitude, GMapConst::MaxLongitude);

			double x = (lng + 180.0) / 360;
			double sinLatitude = sin(lat * GMapConst::PI / 180.0);
			double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4.0 * GMapConst::PI);

			unsigned long mapSize = 256 << zoom;

			*px = (long)clip(x * mapSize + 0.5, 0, mapSize - 1);
			*py = (long)clip(y * mapSize + 0.5, 0, mapSize - 1);
		}

		double measure(double lat1, double lng1, double lat2, double lng2)
		{
			const double R = 6378.137; // Radius of earth in KM

			double dLat = lat2 * GMapConst::PI / 180.0 - lat1 * GMapConst::PI / 180.0;
			double dLon = lng2 * GMapConst::PI / 180.0 - lng1 * GMapConst::PI / 180.0;

			double a = sin(dLat * 0.5) * sin(dLat * 0.5) 
				+ cos(lat1 * GMapConst::PI / 180.0) * cos(lat2 * GMapConst::PI / 180) 
				* sin(dLon * 0.5) * sin(dLon * 0.5);

			double c = 2.0 * atan2(sqrt(a), sqrt(1 - a));
			double d = R * c;

			return d; // km
		}
	}
}