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
#include "CGUIParticle.h"

#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/CGUIFactory.h"
#include "GameObject/CZone.h"
#include "Camera/CCamera.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	namespace Particle
	{
		void CGUIParticle::registerPlugin()
		{
			CGUIFactory::getInstance()->addPlugin("CGUIParticle", [](CGUIElement* parent)->CGUIElement*
				{
					return new CGUIParticle(parent->getCanvas(), parent);
				});
		}

		CGUIParticle::CGUIParticle(CCanvas* canvas, CGUIElement* parent) :
			CGUIElement(canvas, parent),
			m_ps(NULL),
			m_autoPlay(true),
			m_particleScale(100.0f),
			m_depthZ(10.0f)
		{

		}

		CGUIParticle::CGUIParticle(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
			CGUIElement(canvas, parent, rect),
			m_ps(NULL),
			m_autoPlay(true),
			m_particleScale(100.0f),
			m_depthZ(10.0f)
		{

		}

		CGUIParticle::~CGUIParticle()
		{
			if (m_ps)
				m_ps->getGameObject()->remove();
		}

		void CGUIParticle::update(CCamera* camera)
		{
			CGUIElement::update(camera);

			if (m_ps)
			{
				m_billboardUp = camera ? camera->getUpVector() : Transform::Oy;
				m_billboardLook = camera ? camera->getLookVector() : Transform::Oz;
				m_billboardLook.Y = m_billboardLook.Y - 0.001f;

				CShaderParticle::setViewUp(m_billboardUp);
				CShaderParticle::setViewLook(m_billboardLook);

				CParticleBufferData* data = m_ps->getData();
				for (u32 i = 0, n = data->AllGroups.size(); i < n; i++)
				{
					Particle::CGroup* group = data->AllGroups[i];

					if (group->getFrameUpdate() != m_ps->getFrameUpdated())
					{
						group->update();
						group->updateFrame(m_ps->getFrameUpdated());
					}

					group->updateForRenderer();
				}
			}
		}

		void CGUIParticle::render(CCamera* camera)
		{
			CGUIElement::render(camera);

			if (m_ps)
			{
				IVideoDriver* driver = getVideoDriver();

				CGraphics2D* graphics = CGraphics2D::getInstance();
				graphics->flush();

				core::matrix4 defaultWorld = driver->getTransform(video::ETS_WORLD);
				core::matrix4 world = defaultWorld * m_transform->World;

				core::matrix4 local;
				local.setScale(core::vector3df(m_particleScale, -m_particleScale, m_particleScale));

				const core::rectf& r = m_guiTransform->Rect;

				local.setTranslation(core::vector3df(
					r.UpperLeftCorner.X + r.getWidth() * 0.5f,
					r.UpperLeftCorner.Y + r.getHeight() * 0.5f,
					m_depthZ)
				);
				world *= local;

				driver->setTransform(video::ETS_WORLD, world);

				CParticleBufferData* data = m_ps->getData();
				for (u32 i = 0, n = data->AllGroups.size(); i < n; i++)
					renderParticleBuffer(data->AllGroups[i]);

				driver->setTransform(video::ETS_WORLD, defaultWorld);
			}
		}

		void CGUIParticle::renderParticleBuffer(Particle::CGroup* g)
		{
			IRenderer* renderer = g->getRenderer();
			if (!renderer)
				return;

			IVideoDriver* driver = getVideoDriver();
			CGraphics2D* graphics = CGraphics2D::getInstance();
			IMeshBuffer* buffer = NULL;

			if (g->UseOrientationAsBillboard)
			{
				CShaderParticle::setViewUp(g->OrientationUp);
				CShaderParticle::setViewLook(g->OrientationNormal);
			}
			else
			{
				CShaderParticle::setViewUp(m_billboardUp);
				CShaderParticle::setViewLook(m_billboardLook);
			}

			if (renderer->useInstancing() == true)
			{
				CParticleInstancing* instancing = g->getIntancing();
				if (instancing->getInstanceBuffer() &&
					instancing->getInstanceBuffer()->getVertexCount() > 0)
				{
					buffer = g->getIntancing()->getMeshBuffer();
				}
			}
			else
			{
				buffer = g->getParticleBuffer()->getMeshBuffer();
			}

			if (buffer && buffer->getIndexBuffer()->getIndexCount() > 0)
			{
				CShaderParticle::setOrientationUp(g->OrientationUp);
				CShaderParticle::setOrientationNormal(g->OrientationNormal);

				video::SMaterial& mat = buffer->getMaterial();
				CMaterial* material = renderer->getMaterial();
				material->updateTexture(mat);
				CShaderMaterial::setMaterial(material);

				// sync z buffer
				u8 defaultZ = mat.ZBuffer;
				mat.ZBuffer = graphics->getMaterial().ZBuffer;

				driver->setMaterial(mat);
				driver->drawMeshBuffer(buffer);

				mat.ZBuffer = defaultZ;
			}
		}

		CObjectSerializable* CGUIParticle::createSerializable()
		{
			CObjectSerializable* object = CGUIElement::createSerializable();

			object->autoRelease(new CFilePathProperty(object, "source", m_source.c_str(), "particle"));
			object->autoRelease(new CFloatProperty(object, "particleScale", m_particleScale, 1.0f, 1000.0f));
			object->autoRelease(new CFloatProperty(object, "depthZ", m_depthZ, -0.0f, 100.0f));
			object->autoRelease(new CBoolProperty(object, "autoPlay", m_autoPlay));

			return object;
		}

		void CGUIParticle::loadSerializable(CObjectSerializable* object)
		{
			CGUIElement::loadSerializable(object);

			m_autoPlay = object->get("autoPlay", true);
			m_depthZ = object->get("depthZ", 10.0f);
			m_particleScale = object->get("particleScale", 100.0f);

			std::string source = object->get<std::string>("source", "");
			if (m_source != source && !source.empty())
				setParticle(source.c_str(), m_autoPlay);

			m_source = source;
		}

		void CGUIParticle::setParticle(const char* source, bool autoPlay)
		{
			if (m_ps)
			{
				m_ps->getGameObject()->remove();
				m_ps = NULL;
			}

			Skylicht::CZone* zone = m_canvas->getGameObject()->getZone();

			CGameObject* psObj = zone->createEmptyObject();
			m_ps = psObj->addComponent<CParticleComponent>();
			m_ps->setSourcePath(source);
			m_ps->load();

			if (!autoPlay)
				m_ps->Stop();

			psObj->setName(getID());
			psObj->setVisible(false);

			m_source = source;
		}

		void CGUIParticle::playParticle()
		{
			if (m_ps)
				m_ps->Play();
		}

		void CGUIParticle::stopParticle()
		{
			if (m_ps)
				m_ps->Stop();
		}

		void CGUIParticle::reload()
		{
			if (!m_source.empty())
			{
				std::string s = m_source;
				setParticle(s.c_str(), m_autoPlay);
			}
		}
	}
}