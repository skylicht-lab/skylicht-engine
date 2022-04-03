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
#include "CSkyBox.h"
#include "GameObject/CGameObject.h"

#include "Entity/CEntity.h"
#include "Entity/CEntityManager.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CSkyBox);

	CATEGORY_COMPONENT(CSkyBox, "SkyBox", "Renderer");

	CSkyBox::CSkyBox() :
		m_skyboxData(NULL),
		m_intensity(1.0f),
		m_color(255, 255, 255, 255)
	{

	}

	CSkyBox::~CSkyBox()
	{
	}

	void CSkyBox::initComponent()
	{
		m_skyboxData = m_gameObject->getEntity()->addData<CSkyBoxData>();
		m_gameObject->getEntityManager()->addRenderSystem<CSkyBoxRender>();

		// default
		setColorIntensity(m_color, m_intensity);
		setTexture6("Common/Textures/Sky/PaperMill.png");
	}

	void CSkyBox::updateComponent()
	{

	}

	CObjectSerializable* CSkyBox::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		std::vector<std::string> textureExts = { "tga","png" };

		object->addAutoRelease(new CFilePathProperty(object, "texture", m_texture.c_str(), textureExts));
		object->addAutoRelease(new CColorProperty(object, "color", m_color));
		object->addAutoRelease(new CFloatProperty(object, "intensity", m_intensity, 0.0f, 3.0f));

		return object;
	}

	void CSkyBox::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		m_texture = object->get<std::string>("texture", "");
		m_color = object->get<SColor>("color", SColor(255, 255, 255, 255));
		m_intensity = object->get<float>("intensity", 1.0f);

		setColorIntensity(m_color, m_intensity);

		if (!m_texture.empty())
			setTexture6(m_texture.c_str());
	}

	void CSkyBox::setTexture6(const char* texture)
	{
		m_texture = texture;
		std::string path = texture;

		path = CStringImp::replaceAll(path, "_X1.png", "");
		path = CStringImp::replaceAll(path, "_X2.png", "");
		path = CStringImp::replaceAll(path, "_Y1.png", "");
		path = CStringImp::replaceAll(path, "_Y2.png", "");
		path = CStringImp::replaceAll(path, "_Z1.png", "");
		path = CStringImp::replaceAll(path, "_Z2.png", "");
		path = CStringImp::replaceAll(path, ".png", "");

		CTextureManager* textureMgr = CTextureManager::getInstance();

		std::string x1 = std::string(path) + "_X1.png";
		m_skyboxData->Left = textureMgr->getTexture(x1.c_str());

		std::string x2 = std::string(path) + "_X2.png";
		m_skyboxData->Right = textureMgr->getTexture(x2.c_str());

		std::string y1 = std::string(path) + "_Y1.png";
		m_skyboxData->Top = textureMgr->getTexture(y1.c_str());

		std::string y2 = std::string(path) + "_Y2.png";
		m_skyboxData->Bottom = textureMgr->getTexture(y2.c_str());

		std::string z1 = std::string(path) + "_Z1.png";
		m_skyboxData->Back = textureMgr->getTexture(z1.c_str());

		std::string z2 = std::string(path) + "_Z2.png";
		m_skyboxData->Front = textureMgr->getTexture(z2.c_str());

		m_skyboxData->updateTexture();
	}

	void CSkyBox::setColorIntensity(const SColor& c, float intensity)
	{
		for (int i = 0; i < 6; i++)
		{
			m_skyboxData->Material[i]->setUniform4("uColor", c);
			m_skyboxData->Material[i]->setUniform("uIntensity", intensity);
		}
	}
}