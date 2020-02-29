/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CGlyphFreetype.h"

#if defined(USE_FREETYPE)

// Glyph metrics:
// --------------
//
//                       xmin                     xmax
//                        |                         |
//                        |<-------- width -------->|
//                        |                         |
//              |         +-------------------------+----------------- ymax
//              |         |    ggggggggg   ggggg    |     ^        ^
//              |         |   g:::::::::ggg::::g    |     |        |
//              |         |  g:::::::::::::::::g    |     |        |
//              |         | g::::::ggggg::::::gg    |     |        |
//              |         | g:::::g     g:::::g     |     |        |
//    offsetX  -|-------->| g:::::g     g:::::g     |  offsetY     |
//              |         | g:::::g     g:::::g     |     |        |
//              |         | g::::::g    g:::::g     |     |        |
//              |         | g:::::::ggggg:::::g     |     |        |
//              |         |  g::::::::::::::::g     |     |      height
//              |         |   gg::::::::::::::g     |     |        |
//  baseline ---*---------|---- gggggggg::::::g-----*--------      |
//            / |         |             g:::::g     |              |
//     origin   |         | gggggg      g:::::g     |              |
//              |         | g:::::gg   gg:::::g     |              |
//              |         |  g::::::ggg:::::::g     |              |
//              |         |   gg:::::::::::::g      |              |
//              |         |     ggg::::::ggg        |              |
//              |         |         gggggg          |              v
//              |         +-------------------------+----------------- ymin
//              |                                   |
//              |------------- advanceX ----------->|

namespace Skylicht
{
	CGlyphFreetype::CGlyphFreetype()
	{
		int	error = FT_Init_FreeType(&m_lib);
		if (error)
			os::Printer::log("FreeType provider: can't init FreeType!  error = %d\n", ELL_ERROR);
	}

	CGlyphFreetype::~CGlyphFreetype()
	{
		int error = FT_Done_FreeType(m_lib);
		if (error)
			os::Printer::log("FreeType provider: can't close FreeType!  error = %d\n", ELL_ERROR);
		m_lib = NULL;
	}

	bool CGlyphFreetype::initFont(const char *name, const char *path)
	{
		io::IFileSystem *fs = getIrrlichtDevice()->getFileSystem();
		io::IReadFile *readFile = fs->createAndOpenFile(path);
		if (readFile != NULL)
		{
			/*
			long dataSize = readFile->getSize();
			FT_Byte *data = new FT_Byte[dataSize];
			readFile->read(data, dataSize);

			FT_Face face = NULL;
			FT_Error error = FT_New_Memory_Face(m_lib, data, dataSize, 0, &face);
			if (error != 0)
			{
				delete []data;
				return false;
			}
			*/
			readFile->drop();
			return true;
		}
		return false;
	}

	IImage *CGlyphFreetype::getCharImage(unsigned short code,
		const char *name,
		int fontSize,
		core::rectf *bounds,
		float *advance,
		float *uvX,
		float *uvY)
	{
		return NULL;
	}
}

#endif