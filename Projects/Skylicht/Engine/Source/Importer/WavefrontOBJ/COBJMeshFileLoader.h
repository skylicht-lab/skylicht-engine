// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// Modify by Skylicht
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

#pragma once

#include "pch.h"
#include "RenderMesh/CMesh.h"
#include "RenderMesh/CSkinnedMesh.h"

#include "Importer/IMeshImporter.h"

namespace Skylicht
{
	class COBJMeshFileLoader : public IMeshImporter
	{
	protected:
		std::vector<std::string> m_textureFolder;

	public:
		COBJMeshFileLoader();

		virtual ~COBJMeshFileLoader();

		virtual void addTextureFolder(const char *folder);

		virtual bool loadModel(const char *resource, CEntityPrefab* output, bool normalMap = true, bool texcoord2 = true, bool batching = false);

	private:

		const c8* readTextures(const c8* bufPtr, const c8* const bufEnd);

		const c8* goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);

		const c8* goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);

		const c8* goNextLine(const c8* buf, const c8* const bufEnd);

		u32 copyWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);

		core::stringc copyLine(const c8* inBuf, const c8* const bufEnd);

		const c8* goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);

		const c8* readColor(const c8* bufPtr, video::SColor& color, const c8* const pBufEnd);

		const c8* readVec3(const c8* bufPtr, core::vector3df& vec, const c8* const pBufEnd);

		const c8* readUV(const c8* bufPtr, core::vector2df& vec, const c8* const pBufEnd);

		const c8* readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd);

	};
}