/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSpineResource.h"

using namespace Skylicht;

namespace spine
{
	CSpineResource::CSpineResource() :
		m_textureLoader(NULL),
		m_drawable(NULL),
		m_atlas(NULL),
		m_attachmentLoader(NULL),
		m_skeletonJson(NULL),
		m_skeletonData(NULL)
	{

	}

	CSpineResource::~CSpineResource()
	{
		free();
	}

	bool CSpineResource::loadAtlas(const char* path, const char* folder)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		io::IReadFile* fileAtlas = fs->createAndOpenFile(path);
		if (fileAtlas == NULL)
			return false;

		u32 fileSize = fileAtlas->getSize();
		c8* fileData = new c8[fileSize];
		fileAtlas->read(fileData, fileSize);
		fileAtlas->drop();

		if (m_textureLoader == NULL)
			m_textureLoader = new CTextureLoader();

		if (m_atlas)
			delete m_atlas;

		m_atlas = new spine::Atlas(fileData, fileSize, folder, m_textureLoader);

		delete[]fileData;
		return true;
	}

	bool CSpineResource::loadSkeletonJson(const char* path)
	{
		if (m_atlas == NULL)
		{
			os::Printer::log("[CSpineResource] loadSkeletonJson with NULL atlas");
			return false;
		}

		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IReadFile* fileJson = fs->createAndOpenFile(path);
		if (fileJson == NULL)
			return false;

		if (m_attachmentLoader)
			delete m_attachmentLoader;
		m_attachmentLoader = new spine::AtlasAttachmentLoader(m_atlas);

		if (m_skeletonJson)
			delete m_skeletonJson;
		m_skeletonJson = new spine::SkeletonJson(m_attachmentLoader);

		if (m_drawable)
		{
			delete m_drawable;
			m_drawable = NULL;
		}

		u32 fileSize = fileJson->getSize();
		c8* fileData = new c8[fileSize];
		fileJson->read(fileData, fileSize);
		fileJson->drop();

		m_skeletonData = m_skeletonJson->readSkeletonData(fileData);
		if (m_skeletonData != NULL)
		{
			m_drawable = new spine::CSkeletonDrawable(m_skeletonData);
		}
		else
		{
			os::Printer::log("[CSpineResource] loadSkeletonJson load json failed!");
			os::Printer::log(m_skeletonJson->getError().buffer());
		}

		delete[]fileData;
		return true;
	}

	void CSpineResource::free()
	{
		if (m_skeletonJson)
			delete m_skeletonJson;

		if (m_attachmentLoader)
			delete m_attachmentLoader;

		if (m_atlas)
			delete m_atlas;

		if (m_textureLoader)
			delete m_textureLoader;

		if (m_drawable)
			delete m_drawable;

		m_skeletonJson = NULL;
		m_attachmentLoader = NULL;
		m_atlas = NULL;
		m_textureLoader = NULL;
		m_drawable = NULL;
	}
}