/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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
#include "CWindowConfig.h"

namespace Skylicht
{
    std::string g_savePath;

    void CWindowConfig::setSaveDirectory(const char *path)
    {
        g_savePath = path;
    }

	bool CWindowConfig::loadConfig(u32 &x, u32 &y, u32 &width, u32 &height, bool &maximize)
	{
        std::string windowFile = g_savePath;
        if (!g_savePath.empty())
            windowFile += "/";
        windowFile += "Window.xml";
        
		FILE *file = fopen(windowFile.c_str(), "rt");
		if (file == NULL)
			return false;

		int rx, ry, rw, rh, rm;

		// read first line
		char buffer[1024];
		fgets(buffer, 1024, file);
		sscanf(buffer, "<!-- %d;%d;%d;%d;%d -->\n", &rx, &ry, &rw, &rh, &rm);

		x = (u32)rx;
		y = (u32)ry;
		width = (u32)rw;
		height = (u32)rh;
		maximize = rm == 1;

		return true;
	}

	void CWindowConfig::saveConfig(u32 x, u32 y, u32 width, u32 height, bool maximize)
	{
		if (width == 0 || height == 0)
			return;

		u32 oldX, oldY, oldW, oldH;
		bool oldMaximize;
		std::string data;

		bool loaded = loadConfigAndExtraData(oldX, oldY, oldW, oldH, oldMaximize, data);

        std::string windowFile = g_savePath;
        if (!g_savePath.empty())
            windowFile += "/";
        windowFile += "Window.xml";
        
		FILE *file = fopen(windowFile.c_str(), "wt");
		if (file == NULL)
			return;

		if (maximize && loaded)
		{
			// no need save position
			x = oldX;
			y = oldY;
			width = oldW;
			height = oldH;
		}

		// write window config
		fprintf(file, "<!-- %d;%d;%d;%d;%d -->\n", x, y, width, height, maximize == 1);

		// write old extra data
		if (data.length() > 0)
			fprintf(file, "%s", data.c_str());

		fclose(file);
	}

	void CWindowConfig::saveExtraData(const char *data)
	{
		FILE *file = fopen("Window.xml", "rt");
		if (file == NULL)
			return;

		// read first line
		char buffer[1024];
		fgets(buffer, 1024, file);
		fclose(file);

		// update
		file = fopen("Window.xml", "wt");
		if (file == NULL)
			return;

		fputs(buffer, file);
		fputs(data, file);
		fclose(file);
	}

	bool CWindowConfig::loadConfigAndExtraData(u32 &x, u32 &y, u32 &width, u32 &height, bool &maximize, std::string &data)
	{
		data = "";
		FILE *file = fopen("Window.xml", "rt");
		if (file == NULL)
			return false;

		int rx, ry, rw, rh, rm;

		// read first line
		char buffer[1024];
		fgets(buffer, 1024, file);
		sscanf(buffer, "<!-- %d;%d;%d;%d;%d -->\n", &rx, &ry, &rw, &rh, &rm);

		x = (u32)rx;
		y = (u32)ry;
		width = (u32)rw;
		height = (u32)rh;
		maximize = rm == 1;

		// read extra data
		while (!feof(file))
		{
			if (fgets(buffer, 1024, file) != NULL)
				data += buffer;
		}

		fclose(file);
		return true;
	}
}
