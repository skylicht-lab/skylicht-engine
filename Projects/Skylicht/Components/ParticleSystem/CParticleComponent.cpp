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
#include "CParticleBufferData.h"
#include "CParticleRenderer.h"
#include "CParticleComponent.h"

#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "IndirectLighting/CIndirectLightingData.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Particle
	{
		ACTIVATOR_REGISTER(CParticleComponent);

		CATEGORY_COMPONENT(CParticleComponent, "Particle", "Particle");

		CParticleComponent::CParticleComponent() :
			m_data(NULL)
		{

		}

		CParticleComponent::~CParticleComponent()
		{
			if (m_gameObject)
				m_gameObject->getEntity()->removeData<CParticleBufferData>();
		}

		void CParticleComponent::initComponent()
		{
			CEntity* entity = m_gameObject->getEntity();

			// add particle buffer data
			m_data = entity->addData<CParticleBufferData>();

			// indirect lighting (for CMeshParticleRenderer)
			entity->addData<CIndirectLightingData>()->initSH();

			// add culling
			entity->addData<CCullingData>();
			entity->addData<CCullingBBoxData>();

			// add renderer system
			m_gameObject->getEntityManager()->addRenderSystem<CParticleRenderer>();
		}

		void CParticleComponent::startComponent()
		{
			if (!m_sourcePath.empty())
			{
				load();
			}
		}

		void CParticleComponent::updateComponent()
		{

		}

		CObjectSerializable* CParticleComponent::createSerializable()
		{
			CObjectSerializable* object = CComponentSystem::createSerializable();
			object->autoRelease(new CFilePathProperty(object, "source", m_sourcePath.c_str(), ".particle"));
			return object;
		}

		void CParticleComponent::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
			m_sourcePath = object->get<std::string>("source", "");
		}

		CGroup* CParticleComponent::duplicateGroup(CGroup* group)
		{
			CObjectSerializable* object = group->createSerializable();
			saveGroup(group, object);

			CGroup* newGroup = m_data->createGroup();
			loadGroup(newGroup, object);

			delete object;
			return newGroup;
		}

		void CParticleComponent::loadGroup(Particle::CGroup* group, CObjectSerializable* object)
		{
			group->loadSerializable(object);

			CFactory* factory = getParticleFactory();
			std::wstring name;

			CObjectSerializable* emitters = (CObjectSerializable*)object->getProperty("Emitters");
			if (emitters)
			{
				for (u32 i = 0, n = emitters->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* emitterData = (CObjectSerializable*)emitters->getPropertyID(i);
					name = CStringImp::convertUTF8ToUnicode(emitterData->Name.c_str());

					CEmitter* emitter = factory->createEmitter(name);
					if (emitter)
					{
						group->addEmitter(emitter);
						emitter->loadSerializable(emitterData);

						CObjectSerializable* zoneData = (CObjectSerializable*)emitterData->getProperty("Zone");
						if (zoneData && zoneData->getNumProperty() > 0)
						{
							zoneData = (CObjectSerializable*)zoneData->getPropertyID(0);
							name = CStringImp::convertUTF8ToUnicode(zoneData->Name.c_str());
							CZone* zone = factory->createZone(name);
							if (zone)
							{
								emitter->setZone(zone);
								zone->loadSerializable(zoneData);
							}
						}
					}
				}
			}

			CObjectSerializable* models = (CObjectSerializable*)object->getProperty("Models");
			if (models)
			{
				for (u32 i = 0, n = models->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* modelData = (CObjectSerializable*)models->getPropertyID(i);
					name = CStringImp::convertUTF8ToUnicode(modelData->Name.c_str());

					CModel* model = group->createModel(name);
					if (model)
						model->loadSerializable(modelData);
				}
			}

			CObjectSerializable* renderer = (CObjectSerializable*)object->getProperty("Renderer");
			if (renderer && renderer->getNumProperty() > 0)
			{
				CObjectSerializable* rendererData = (CObjectSerializable*)renderer->getPropertyID(0);
				if (rendererData)
				{
					name = CStringImp::convertUTF8ToUnicode(rendererData->Name.c_str());
					IRenderer* renderer = factory->createRenderer(name);
					if (renderer)
					{
						renderer->loadSerializable(rendererData);
						group->setRenderer(renderer);
					}
				}
			}

			CObjectSerializable* subGroups = (CObjectSerializable*)object->getProperty("SubGroups");
			if (subGroups && subGroups->getNumProperty() > 0)
			{
				for (u32 i = 0, n = subGroups->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* groupData = (CObjectSerializable*)subGroups->getPropertyID(i);
					if (groupData)
					{
						CSubGroup* subGroup = m_data->createSubGroup(group);
						loadGroup(subGroup, groupData);
					}
				}
			}
		}

		CEmitter* CParticleComponent::duplicateEmitter(CGroup* group, CEmitter* emitter)
		{
			CFactory* factory = getParticleFactory();

			CObjectSerializable* object = emitter->createSerializable();
			std::wstring name = CStringImp::convertUTF8ToUnicode(object->Name.c_str());

			CEmitter* newEmitter = factory->createEmitter(name);
			if (newEmitter)
			{
				group->addEmitter(newEmitter);

				newEmitter->loadSerializable(object);
				if (emitter->getZone())
				{
					CObjectSerializable* zoneData = emitter->getZone()->createSerializable();
					name = CStringImp::convertUTF8ToUnicode(zoneData->Name.c_str());

					CZone* newZone = factory->createZone(name);
					newEmitter->setZone(newZone);

					newZone->loadSerializable(zoneData);
					delete zoneData;
				}
			}

			delete object;
			return newEmitter;
		}

		bool CParticleComponent::save()
		{
			io::IXMLWriter* writeFile = getIrrlichtDevice()->getFileSystem()->createXMLWriter(m_sourcePath.c_str());
			if (writeFile == NULL)
				return false;

			writeFile->writeXMLHeader();
			writeFile->writeComment(L"File generated by function CParticleComponent::save");
			writeFile->writeLineBreak();

			CObjectSerializable* object = createSerializable();

			CObjectSerializable* groups = new CObjectSerializable("Groups");
			object->addProperty(groups);
			object->autoRelease(groups);

			saveGroups(groups);

			object->save(writeFile);
			delete object;

			writeFile->drop();
			return true;
		}

		void CParticleComponent::saveGroups(CObjectSerializable* groups)
		{
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				CGroup* group = m_data->Groups[i];
				CObjectSerializable* object = group->createSerializable();

				saveGroup(group, object);

				groups->addProperty(object);
				groups->autoRelease(object);
			}
		}

		void CParticleComponent::saveGroup(CGroup* group, CObjectSerializable* object)
		{
			CObjectSerializable* emitters = new CObjectSerializable("Emitters");
			object->addProperty(emitters);
			object->autoRelease(emitters);

			for (CEmitter* emitter : group->getEmitters())
			{
				CObjectSerializable* emitterData = emitter->createSerializable();
				emitters->addProperty(emitterData);
				emitters->autoRelease(emitterData);

				CObjectSerializable* zone = new CObjectSerializable("Zone");
				emitterData->addProperty(zone);
				emitterData->autoRelease(zone);

				if (emitter->getZone())
				{
					CObjectSerializable* zoneData = emitter->getZone()->createSerializable();
					zone->addProperty(zoneData);
					zone->autoRelease(zoneData);
				}
			}

			CObjectSerializable* models = new CObjectSerializable("Models");
			object->addProperty(models);
			object->autoRelease(models);

			for (CModel* model : group->getModels())
			{
				CObjectSerializable* emitterData = model->createSerializable();
				models->addProperty(emitterData);
				models->autoRelease(emitterData);
			}

			CObjectSerializable* renderer = new CObjectSerializable("Renderer");
			object->addProperty(renderer);
			object->autoRelease(renderer);

			if (group->getRenderer())
			{
				CObjectSerializable* rendererData = group->getRenderer()->createSerializable();
				renderer->addProperty(rendererData);
				renderer->autoRelease(rendererData);
			}

			std::vector<Particle::CSubGroup*> subGroups = m_data->getSubGroup(group);
			if (subGroups.size() > 0)
			{
				CObjectSerializable* groups = new CObjectSerializable("SubGroups");
				object->addProperty(groups);
				object->autoRelease(groups);

				for (Particle::CSubGroup* g : subGroups)
				{
					CObjectSerializable* groupData = g->createSerializable();
					groups->addProperty(groupData);
					groups->autoRelease(groupData);

					saveGroup(g, groupData);
				}
			}
		}

		bool CParticleComponent::load()
		{
			io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(m_sourcePath.c_str());
			if (reader == NULL)
				return false;

			std::wstring nodeName = L"node";
			std::wstring attributeName;

			m_data->clear();

			CGroup* group = NULL;

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						if (reader->getAttributeValue(L"type") != NULL)
						{
							attributeName = reader->getAttributeValue(L"type");

							if (attributeName == L"CParticleComponent")
							{
								std::string p = m_sourcePath;
								CObjectSerializable* data = createSerializable();
								data->parseSerializable(reader);
								loadSerializable(data);
								delete data;
								m_sourcePath = p;
							}
							else if (attributeName == L"CGroup")
							{
								group = m_data->createGroup();
								CObjectSerializable* data = group->createSerializable();
								data->parseSerializable(reader);
								group->loadSerializable(data);
								delete data;
								loadGroup(group, reader);
							}
						}
					}
					break;
				default:
					break;
				}
			}

			reader->drop();
			return false;
		}

		void CParticleComponent::loadGroup(Particle::CGroup* group, io::IXMLReader* reader)
		{
			std::wstring nodeName = L"node";
			std::wstring attributeName;

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						attributeName = reader->getAttributeValue(L"type");

						if (attributeName == L"Emitters")
						{
							loadEmitters(group, reader);
						}
						else if (attributeName == L"Models")
						{
							loadModels(group, reader);
						}
						else if (attributeName == L"Renderer")
						{
							loadRenderer(group, reader);
						}
						else if (attributeName == L"SubGroups")
						{
							loadSubGroups(group, reader);
						}
					}
					break;
				case io::EXN_ELEMENT_END:
				{
					if (nodeName == reader->getNodeName())
					{
						return;
					}
				}
				break;
				}
			}
		}

		void CParticleComponent::loadEmitters(CGroup* group, io::IXMLReader* reader)
		{
			std::wstring nodeName = L"node";
			std::wstring attributeName;
			int pos = 1;

			CEmitter* emitter = NULL;
			CZone* zone = NULL;
			CFactory* factory = getParticleFactory();

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						pos++;
						if (reader->getAttributeValue(L"type") != NULL)
						{
							attributeName = reader->getAttributeValue(L"type");

							if (!emitter)
							{
								emitter = factory->createEmitter(attributeName);
								if (emitter)
								{
									group->addEmitter(emitter);

									CObjectSerializable* data = emitter->createSerializable();
									data->parseSerializable(reader);
									emitter->loadSerializable(data);
									delete data;
								}
							}
							else
							{
								zone = factory->createZone(attributeName);
								if (zone)
								{
									emitter->setZone(zone);

									CObjectSerializable* data = zone->createSerializable();
									data->parseSerializable(reader);
									zone->loadSerializable(data);
									delete data;
								}
							}
						}
					}
					break;
				case io::EXN_ELEMENT_END:
					if (nodeName == reader->getNodeName())
					{
						pos--;
						if (pos == 1)
						{
							emitter = NULL;
							zone = NULL;
						}
					}

					if (pos == 0)
						return;

					break;
				}
			}
		}

		void CParticleComponent::loadModels(CGroup* group, io::IXMLReader* reader)
		{
			std::wstring nodeName = L"node";
			std::wstring attributeName;
			int pos = 1;

			CFactory* factory = getParticleFactory();

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						pos++;
						if (reader->getAttributeValue(L"type") != NULL)
						{
							attributeName = reader->getAttributeValue(L"type");

							CModel* model = group->createModel(attributeName);
							if (model)
							{
								CObjectSerializable* data = model->createSerializable();
								data->parseSerializable(reader);
								model->loadSerializable(data);
								delete data;
							}
						}
					}
					break;
				case io::EXN_ELEMENT_END:
					if (nodeName == reader->getNodeName())
						pos--;

					if (pos == 0)
						return;

					break;
				}
			}
		}

		void CParticleComponent::loadRenderer(CGroup* group, io::IXMLReader* reader)
		{
			std::wstring nodeName = L"node";
			std::wstring attributeName;
			int pos = 1;

			CFactory* factory = getParticleFactory();

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						pos++;
						if (reader->getAttributeValue(L"type") != NULL)
						{
							attributeName = reader->getAttributeValue(L"type");

							IRenderer* renderer = factory->createRenderer(attributeName);
							if (renderer)
							{
								CObjectSerializable* data = renderer->createSerializable();
								data->parseSerializable(reader);
								renderer->loadSerializable(data);
								delete data;

								group->setRenderer(renderer);
							}
						}
					}
					break;
				case io::EXN_ELEMENT_END:
					if (nodeName == reader->getNodeName())
						pos--;

					if (pos == 0)
						return;

					break;
				}
			}
		}

		void CParticleComponent::loadSubGroups(Particle::CGroup* group, io::IXMLReader* reader)
		{
			std::wstring nodeName = L"node";
			std::wstring attributeName;
			int pos = 1;

			while (reader->read())
			{
				switch (reader->getNodeType())
				{
				case io::EXN_ELEMENT:
					if (nodeName == reader->getNodeName())
					{
						pos++;
						if (reader->getAttributeValue(L"type") != NULL)
						{
							attributeName = reader->getAttributeValue(L"type");
							if (attributeName == L"CSubGroup")
							{
								CSubGroup* newGroup = m_data->createSubGroup(group);
								CObjectSerializable* data = newGroup->createSerializable();
								data->parseSerializable(reader);
								newGroup->loadSerializable(data);
								delete data;
								loadGroup(newGroup, reader);
							}
						}
					}
					break;
				case io::EXN_ELEMENT_END:
					if (nodeName == reader->getNodeName())
						pos--;

					if (pos == 0)
						return;

					break;
				}
			}
		}

		void CParticleComponent::setGroupTransform(const core::matrix4& world)
		{
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				m_data->Groups[i]->setWorldMatrix(world);
			}
		}

		CGroup* CParticleComponent::createParticleGroup()
		{
			return m_data->createGroup();
		}

		CSubGroup* CParticleComponent::createParticleSubGroup(CGroup* group)
		{
			return m_data->createSubGroup(group);
		}

		void CParticleComponent::removeParticleGroup(CGroup* group)
		{
			m_data->removeGroup(group);
		}

		void CParticleComponent::Play()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				std::vector<CEmitter*>& emitter = groups[i]->getEmitters();
				for (CEmitter* e : emitter)
				{
					e->resetTank();
				}
			}
		}

		void CParticleComponent::Stop()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				std::vector<CEmitter*>& emitter = groups[i]->getEmitters();
				for (CEmitter* e : emitter)
				{
					e->stop();
				}
			}
		}

		bool CParticleComponent::IsPlaying()
		{
			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
			{
				if (groups[i]->getNumParticles() > 0)
					return true;
			}

			return false;
		}

		u32 CParticleComponent::getTotalParticle()
		{
			u32 n = 0;

			CGroup** groups = m_data->Groups.pointer();
			for (u32 i = 0, n = m_data->Groups.size(); i < n; i++)
				n += groups[i]->getNumParticles();

			return n;
		}
	}
}