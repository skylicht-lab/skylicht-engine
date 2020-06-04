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
#include "RenderPipeline/CBaseRP.h"

#include "CReflectionProbeData.h"
#include "CReflectionProbeRender.h"
#include "CReflectionProbe.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	CReflectionProbe::CReflectionProbe() :
		m_staticTexture(NULL),
		m_dynamicTexture(NULL),
		m_bakeSize(512, 512),
		m_probeData(NULL)
	{
		for (int i = 0; i < 6; i++)
			m_bakeTexture[i] = NULL;
	}

	CReflectionProbe::~CReflectionProbe()
	{
		removeBakeTexture();
		removeDynamicTexture();
	}

	void CReflectionProbe::removeBakeTexture()
	{
		for (int i = 0; i < 6; i++)
		{
			if (m_bakeTexture[i] != NULL)
			{
				getVideoDriver()->removeTexture(m_bakeTexture[i]);
				m_bakeTexture[i] = NULL;
			}
		}
	}

	void CReflectionProbe::removeDynamicTexture()
	{
		if (m_dynamicTexture == NULL)
		{
			getVideoDriver()->removeTexture(m_dynamicTexture);
			m_dynamicTexture = NULL;
		}
	}

	void CReflectionProbe::initComponent()
	{
		m_probeData = m_gameObject->getEntity()->addData<CReflectionProbeData>();
		m_gameObject->getEntityManager()->addRenderSystem<CReflectionProbeRender>();
	}

	void CReflectionProbe::updateComponent()
	{

	}

	void CReflectionProbe::bakeProbe(CCamera *camera, IRenderPipeline *rp, CEntityManager *entityMgr)
	{
		if (m_dynamicTexture == NULL)
			m_dynamicTexture = getVideoDriver()->addRenderTargetCubeTexture(m_bakeSize, "bake_cube_reflection", video::ECF_A8R8G8B8);

		core::vector3df position = m_gameObject->getPosition();
		CBaseRP *baseRP = dynamic_cast<CBaseRP*>(rp);
		if (baseRP != NULL)
		{
			baseRP->renderCubeEnvironment(camera, entityMgr, position, m_dynamicTexture, NULL, 0);

			m_dynamicTexture->regenerateMipMapLevels();
		}
	}

	void CReflectionProbe::bakeProbeToFile(CCamera *camera, IRenderPipeline *rp, CEntityManager *entityMgr, const char *outfolder, const char *outname)
	{
		for (int i = 0; i < 6; i++)
		{
			if (m_bakeTexture[i] != NULL)
				m_bakeTexture[i] = getVideoDriver()->addRenderTargetTexture(m_bakeSize, "bake_reflection", video::ECF_A8R8G8B8);
		}

		core::vector3df position = m_gameObject->getPosition();
		CBaseRP *baseRP = dynamic_cast<CBaseRP*>(rp);
		if (baseRP != NULL)
		{
			baseRP->renderEnvironment(camera, entityMgr, position, m_bakeTexture, NULL, 0);

			const char *cubeText[] =
			{
				"X1",
				"X2",
				"Y1",
				"Y2",
				"Z1",
				"Z2"
			};

			char name[512];
			for (int i = 0; i < 6; i++)
			{
				sprintf(name, "%s/%s_%s.png", outfolder, outname, cubeText[i]);
				void *imageData = m_bakeTexture[i]->lock(video::ETLM_READ_ONLY);

				IImage* im = getVideoDriver()->createImageFromData(
					m_bakeTexture[i]->getColorFormat(),
					m_bakeTexture[i]->getSize(),
					imageData);

				if (getVideoDriver()->getDriverType() == video::EDT_DIRECT3D11)
					im->swapBG();

				getVideoDriver()->writeImageToFile(im, name);
				im->drop();

				m_bakeTexture[i]->unlock();
			}
		}
	}

	bool CReflectionProbe::loadStaticTexture(const char *path)
	{
		std::string x1 = std::string(path) + "_X1.png";
		std::string x2 = std::string(path) + "_X2.png";
		std::string y1 = std::string(path) + "_Y1.png";
		std::string y2 = std::string(path) + "_Y2.png";
		std::string z1 = std::string(path) + "_Z1.png";
		std::string z2 = std::string(path) + "_Z2.png";

		m_staticTexture = CTextureManager::getInstance()->getCubeTexture(
			x1.c_str(), x2.c_str(),
			y1.c_str(), y2.c_str(),
			z1.c_str(), z2.c_str()
		);

		if (m_staticTexture != NULL)
			m_staticTexture->regenerateMipMapLevels();

		return (m_staticTexture != NULL);
	}
}