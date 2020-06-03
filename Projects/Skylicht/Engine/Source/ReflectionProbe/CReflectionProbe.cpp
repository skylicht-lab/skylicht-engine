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
#include "GameObject/CGameObject.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityManager.h"

#include "RenderPipeline/IRenderPipeline.h"
#include "CReflectionProbe.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	CReflectionProbe::CReflectionProbe() :
		m_staticTexture(NULL),
		m_bakeTexture(NULL)
	{

	}

	CReflectionProbe::~CReflectionProbe()
	{

	}

	void CReflectionProbe::initComponent()
	{

	}

	void CReflectionProbe::updateComponent()
	{

	}

	void CReflectionProbe::bakeProbe(CCamera *camera, IRenderPipeline *rp, CEntityManager *entityMgr)
	{
		core::vector3df position = m_gameObject->getPosition();

	}

	bool CReflectionProbe::loadStaticTexture(const char *path)
	{
		std::string x1 = std::string(path) + "_X1.tga";
		std::string x2 = std::string(path) + "_X2.tga";
		std::string y1 = std::string(path) + "_Y1.tga";
		std::string y2 = std::string(path) + "_Y2.tga";
		std::string z1 = std::string(path) + "_Z1.tga";
		std::string z2 = std::string(path) + "_Z2.tga";

		m_staticTexture = CTextureManager::getInstance()->getCubeTexture(
			x1.c_str(), x2.c_str(),
			y1.c_str(), y2.c_str(),
			z1.c_str(), z2.c_str()
		);

		return (m_staticTexture != NULL);
	}
}