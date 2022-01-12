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
#include "CSkyDome.h"
#include "GameObject/CGameObject.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityManager.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CSkyDome);

	CATEGORY_COMPONENT(CSkyDome, "SkyDome", "Renderer");

	CSkyDome::CSkyDome() :
		m_skyDomeData(NULL),
		m_intensity(1.0f),
		m_color(255, 255, 255, 255)
	{

	}

	CSkyDome::~CSkyDome()
	{
	}

	void CSkyDome::initComponent()
	{
		m_skyDomeData = m_gameObject->getEntity()->addData<CSkyDomeData>();
		m_gameObject->getEntityManager()->addRenderSystem<CSkyDomeRender>();

		// default
		m_texture = "Common/Textures/Sky/Helipad.png";
		ITexture* texture = CTextureManager::getInstance()->getTexture(m_texture.c_str());
		if (texture != NULL)
			setData(texture, m_color, m_intensity);
	}

	void CSkyDome::updateComponent()
	{

	}

	CObjectSerializable* CSkyDome::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		std::vector<std::string> textureExts = { "tga","png" };

		object->addAutoRelease(new CFilePathProperty(object, "texture", m_texture.c_str(), textureExts));
		object->addAutoRelease(new CColorProperty(object, "color", m_color));
		object->addAutoRelease(new CFloatProperty(object, "intensity", m_intensity, 0.0f, 3.0f));

		return object;
	}

	void CSkyDome::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_texture = object->get<std::string>("texture", "");
		m_color = object->get<SColor>("color", SColor(255, 255, 255, 255));
		m_intensity = object->get<float>("intensity", 1.0f);

		if (!m_texture.empty())
		{
			ITexture* texture = CTextureManager::getInstance()->getTexture(m_texture.c_str());
			if (texture != NULL)
				setData(texture, m_color, m_intensity);
		}
	}

	void CSkyDome::setData(ITexture* texture, const SColor& c, float intensity)
	{
		m_skyDomeData->SkyDomeMaterial->setTexture(0, texture);
		m_skyDomeData->SkyDomeMaterial->setUniform4("uColor", c);
		m_skyDomeData->SkyDomeMaterial->setUniform("uIntensity", intensity);
		m_skyDomeData->SkyDomeMaterial->applyMaterial();
	}
}