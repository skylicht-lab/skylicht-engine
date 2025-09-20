/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "IRenderer.h"
#include "Utils/CStringImp.h"

#include "Material/CMaterialManager.h"
#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	namespace Particle
	{
		IRenderer::IRenderer(ERenderer type) :
			m_type(type),
			m_material(NULL),
			m_customMaterial(NULL),
			m_useCustomMaterial(false),
			SizeX(1.0f),
			SizeY(1.0f),
			SizeZ(1.0f),
			m_atlasNx(1),
			m_atlasNy(1),
			m_useInstancing(true),
			m_emission(false),
			m_ztest(true),
			m_emissionIntensity(1.0f),
			m_needUpdateMesh(true)
		{

		}

		IRenderer::~IRenderer()
		{

		}

		CMaterial* IRenderer::getMaterial()
		{
			if (m_useCustomMaterial && m_customMaterial)
				return m_customMaterial;

			return m_material;
		}

		void IRenderer::setTexture(int slot, ITexture* texture)
		{
			m_material->setTexture(slot, texture);
			m_material->setManualInitTexture(true);

			if (m_customMaterial)
			{
				m_customMaterial->setTexture(slot, texture);
				m_customMaterial->setManualInitTexture(true);
			}
		}

		void IRenderer::setAtlas(u32 x, u32 y)
		{
			m_atlasNx = x;
			m_atlasNy = y;

			if (m_atlasNx < 1)
				m_atlasNx = 1;

			if (m_atlasNy < 1)
				m_atlasNy = 1;
		}

		CObjectSerializable* IRenderer::createSerializable()
		{
			CObjectSerializable* object = CParticleSerializable::createSerializable();

			CStringProperty* name = new CStringProperty(object, "name", CStringImp::convertUnicodeToUTF8(getName()).c_str());
			name->setHidden(true);
			object->autoRelease(name);

			object->autoRelease(new CBoolProperty(object, "emission", m_emission));
			object->autoRelease(new CFloatProperty(object, "emissionIntensity", m_emissionIntensity, 0.0f, 10.0f));

			object->autoRelease(new CFloatProperty(object, "sizeX", SizeX, 0.0f));
			object->autoRelease(new CFloatProperty(object, "sizeY", SizeY, 0.0f));
			object->autoRelease(new CFloatProperty(object, "sizeZ", SizeZ, 0.0f));

			CImageSourceProperty* img = new CImageSourceProperty(object, "texture", m_texturePath.c_str());
			img->setUISpace(10.0f);
			object->autoRelease(img);

			object->autoRelease(new CIntProperty(object, "atlasX", m_atlasNx, 1));
			object->autoRelease(new CIntProperty(object, "atlasY", m_atlasNy, 1));

			object->autoRelease(new CBoolProperty(object, "ztest", m_ztest));

			CBoolProperty* useCustom = new CBoolProperty(object, "custom material", m_useCustomMaterial);
			useCustom->setUISpace(10.0f);
			object->autoRelease(useCustom);

			CFilePathProperty* material = new CFilePathProperty(object, "material", m_materialPath.c_str(), CMaterialManager::getMaterialExts());
			material->setUIHeader("Custom Material");
			object->autoRelease(material);

			return object;
		}

		void IRenderer::loadSerializable(CObjectSerializable* object)
		{
			CParticleSerializable::loadSerializable(object);

			bool forceUpdateMaterial = false;
			bool useMaterial = !m_materialPath.empty();
			bool useCustom = m_useCustomMaterial;

			m_emission = object->get<bool>("emission", true);
			m_emissionIntensity = object->get<float>("emissionIntensity", 1.0f);

			SizeX = object->get<float>("sizeX", 1.0f);
			SizeY = object->get<float>("sizeY", 1.0f);
			SizeZ = object->get<float>("sizeZ", 1.0f);

			m_ztest = object->get<bool>("ztest", true);

			CImageSourceProperty* texturePath = dynamic_cast<CImageSourceProperty*>(object->getProperty("texture"));
			std::string src;
			if (texturePath)
				src = texturePath->get();

			if (src != m_texturePath)
				setTexturePath(src.c_str());

			m_atlasNx = object->get<int>("atlasX", 1);
			m_atlasNy = object->get<int>("atlasY", 1);

			m_useCustomMaterial = object->get<bool>("custom material", false);

			m_materialPath = object->get<std::string>("material", std::string());
			if (!m_materialPath.empty() && !useMaterial)
			{
				// force enable material while update new material in editor
				m_useCustomMaterial = true;
				forceUpdateMaterial = true;
			}

			if (forceUpdateMaterial || m_useCustomMaterial != useCustom)
			{
				if (m_useCustomMaterial == true && !m_materialPath.empty())
				{
					m_customMaterial = NULL;

					ArrayMaterial& loadMaterials = CMaterialManager::getInstance()->loadMaterial(m_materialPath.c_str(), true, std::vector<std::string>());
					if (loadMaterials.size() > 0)
						m_customMaterial = loadMaterials[0];
				}
			}

			if (m_material)
			{
				m_material->setZTest(m_ztest ? ECFN_LESSEQUAL : ECFN_DISABLED);
				m_material->updateShaderParams();
			}

			if (m_customMaterial)
			{
				m_customMaterial->setZTest(m_ztest ? ECFN_LESSEQUAL : ECFN_DISABLED);
				m_customMaterial->updateShaderParams();
			}
		}

		void IRenderer::setTexturePath(const char* path)
		{
			m_texturePath = path;
			ITexture* t = CTextureManager::getInstance()->getTexture(path);
			setTexture(0, t);
		}
	}
}