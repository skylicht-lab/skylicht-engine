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

#include "CReflectionProbeData.h"
#include "CReflectionProbeRender.h"
#include "CReflectionProbe.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CReflectionProbe);

	CATEGORY_COMPONENT(CReflectionProbe, "Reflection Probe", "Indirect Lighting");

	CReflectionProbe::CReflectionProbe() :
		m_staticTexture(NULL),
		m_bakedTexture(NULL),
		m_size(EReflectionSize::X512),
		m_bakeSize(512, 512),
		m_probeData(NULL),
		m_type(EReflectionType::Baked)
	{
		for (int i = 0; i < 6; i++)
			m_bakeTexture[i] = NULL;
	}

	CReflectionProbe::~CReflectionProbe()
	{
		removeBakeTexture();
		removeStaticTexture();

		if (m_gameObject)
			m_gameObject->getEntity()->removeData<CReflectionProbeData>();
	}

	void CReflectionProbe::removeBakeTexture()
	{
		if (m_bakedTexture != NULL)
			getVideoDriver()->removeTexture(m_bakedTexture);

		for (int i = 0; i < 6; i++)
		{
			if (m_bakeTexture[i] != NULL)
			{
				getVideoDriver()->removeTexture(m_bakeTexture[i]);
				m_bakeTexture[i] = NULL;
			}
		}
	}

	void CReflectionProbe::removeStaticTexture()
	{
		if (m_staticTexture != NULL)
		{
			CTextureManager::getInstance()->removeTexture(m_staticTexture);
			m_staticTexture = NULL;
		}
	}

	void CReflectionProbe::initComponent()
	{
		m_probeData = m_gameObject->getEntity()->addData<CReflectionProbeData>();
		m_gameObject->getEntityManager()->addRenderSystem<CReflectionProbeRender>();
	}

	void CReflectionProbe::updateComponent()
	{
		ITexture* oldTexture = m_probeData->ReflectionTexture;

		if (m_type == EReflectionType::Static)
			m_probeData->ReflectionTexture = m_staticTexture;
		else
			m_probeData->ReflectionTexture = m_bakedTexture;

		if (m_probeData->ReflectionTexture != oldTexture)
			m_probeData->Invalidate = true;
	}

	CObjectSerializable* CReflectionProbe::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CEnumProperty<EReflectionType>* enumType = new CEnumProperty<EReflectionType>(object, "type", m_type);
		enumType->addEnumString("Static", EReflectionType::Static);
		enumType->addEnumString("Bake", EReflectionType::Baked);
		object->autoRelease(enumType);

		CEnumProperty<EReflectionSize>* bakeSizeType = new CEnumProperty<EReflectionSize>(object, "size", m_size);
		bakeSizeType->setUIHeader("Bake Probe");
		bakeSizeType->addEnumString("256", EReflectionSize::X256);
		bakeSizeType->addEnumString("512", EReflectionSize::X512);
		bakeSizeType->addEnumString("1024", EReflectionSize::X1024);
		bakeSizeType->addEnumString("2048", EReflectionSize::X2048);
		object->autoRelease(bakeSizeType);

		std::vector<std::string> exts;
		exts.push_back("tga");
		exts.push_back("png");
		CFilePathProperty* staticReflection = new CFilePathProperty(object, "static", m_staticPath.c_str(), exts);
		staticReflection->setUIHeader("Static Probe");
		object->autoRelease(staticReflection);

		return object;
	}

	void CReflectionProbe::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_type = object->get<EReflectionType>("type", EReflectionType::Baked);
		m_size = object->get<EReflectionSize>("size", EReflectionSize::X512);

		std::string staticPath = object->get<std::string>("static", "");
		if (!staticPath.empty() && m_staticPath != staticPath)
		{
			m_staticPath = staticPath;
			std::string path = staticPath;

			path = CStringImp::replaceAll(path, "_X1.png", "");
			path = CStringImp::replaceAll(path, "_X2.png", "");
			path = CStringImp::replaceAll(path, "_Y1.png", "");
			path = CStringImp::replaceAll(path, "_Y2.png", "");
			path = CStringImp::replaceAll(path, "_Z1.png", "");
			path = CStringImp::replaceAll(path, "_Z2.png", "");
			path = CStringImp::replaceAll(path, ".png", "");

			loadStaticTexture(path.c_str());
		}
	}

	void CReflectionProbe::bakeProbe(CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr)
	{
		core::dimension2du targetSize;
		switch (m_size)
		{
		case EReflectionSize::X256:
			targetSize.set(256, 256);
			break;
		case EReflectionSize::X512:
			targetSize.set(512, 512);
			break;
		case EReflectionSize::X1024:
			targetSize.set(1024, 1024);
			break;
		case EReflectionSize::X2048:
			targetSize.set(2048, 2048);
			break;
		}

		if (targetSize != m_bakeSize)
		{
			m_bakeSize = targetSize;
			if (m_bakedTexture)
			{
				if (m_probeData->ReflectionTexture == m_bakedTexture)
					m_probeData->ReflectionTexture = NULL;

				getVideoDriver()->removeTexture(m_bakedTexture);
				m_bakedTexture = NULL;
			}
		}

		if (m_bakedTexture == NULL)
			m_bakedTexture = getVideoDriver()->addRenderTargetCubeTexture(m_bakeSize, "bake_cube_reflection", video::ECF_A8R8G8B8);

		core::vector3df position = m_gameObject->getPosition();
		CBaseRP* baseRP = dynamic_cast<CBaseRP*>(rp);
		if (baseRP != NULL)
		{
			baseRP->renderCubeEnvironment(camera, entityMgr, position, m_bakedTexture, NULL, 0, true);
			m_bakedTexture->regenerateMipMapLevels();
			m_probeData->ReflectionTexture = m_bakedTexture;
			m_type = EReflectionType::Baked;
		}
	}

	void CReflectionProbe::bakeProbeToFile(CCamera* camera, IRenderPipeline* rp, CEntityManager* entityMgr, const char* outfolder, const char* outname)
	{
		core::dimension2du targetSize;
		switch (m_size)
		{
		case EReflectionSize::X256:
			targetSize.set(256, 256);
			break;
		case EReflectionSize::X512:
			targetSize.set(512, 512);
			break;
		case EReflectionSize::X1024:
			targetSize.set(1024, 1024);
			break;
		case EReflectionSize::X2048:
			targetSize.set(2048, 2048);
			break;
		}

		if (targetSize != m_bakeSize)
		{
			m_bakeSize = targetSize;
			for (int i = 0; i < 6; i++)
			{
				if (m_bakeTexture[i] != NULL)
				{
					getVideoDriver()->removeTexture(m_bakeTexture[i]);
					m_bakeTexture[i] = NULL;
				}
			}
		}

		for (int i = 0; i < 6; i++)
		{
			if (m_bakeTexture[i] == NULL)
				m_bakeTexture[i] = getVideoDriver()->addRenderTargetTexture(m_bakeSize, "bake_reflection", video::ECF_A8R8G8B8);
		}

		core::vector3df position = m_gameObject->getPosition();
		CBaseRP* baseRP = dynamic_cast<CBaseRP*>(rp);
		if (baseRP != NULL)
		{
			baseRP->renderEnvironment(camera, entityMgr, position, m_bakeTexture, NULL, 0);

			const char* cubeText[] =
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
				void* imageData = m_bakeTexture[i]->lock(video::ETLM_READ_ONLY);

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

	video::ITexture* CReflectionProbe::getReflectionTexture()
	{
		return m_probeData->ReflectionTexture;
	}

	bool CReflectionProbe::loadStaticTexture(const char* path)
	{
		std::string x1 = std::string(path) + "_X1.png";
		std::string x2 = std::string(path) + "_X2.png";
		std::string y1 = std::string(path) + "_Y1.png";
		std::string y2 = std::string(path) + "_Y2.png";
		std::string z1 = std::string(path) + "_Z1.png";
		std::string z2 = std::string(path) + "_Z2.png";

		if (m_staticTexture != NULL)
		{
			CTextureManager::getInstance()->removeTexture(m_staticTexture);
			m_staticTexture = NULL;
		}

		m_staticTexture = CTextureManager::getInstance()->getCubeTexture(
			x1.c_str(), x2.c_str(),
			y1.c_str(), y2.c_str(),
			z1.c_str(), z2.c_str()
		);

		if (m_staticTexture != NULL)
		{
			m_staticTexture->regenerateMipMapLevels();
			m_probeData->ReflectionTexture = m_staticTexture;
			m_type = EReflectionType::Static;
		}

		return (m_staticTexture != NULL);
	}
}