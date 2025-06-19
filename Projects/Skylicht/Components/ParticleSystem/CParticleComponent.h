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

#pragma once

#include "Components/CComponentSystem.h"
#include "Particles/CFactory.h"
#include "CParticleBufferData.h"

namespace Skylicht
{
	namespace Particle
	{
		class COMPONENT_API CParticleComponent : public CComponentSystem
		{
		protected:
			CParticleBufferData* m_data;

			CFactory m_factory;

			std::string m_sourcePath;

		public:
			CParticleComponent();

			virtual ~CParticleComponent();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

		public:

			CFactory* getParticleFactory()
			{
				return &m_factory;
			}

			inline CParticleBufferData* getData()
			{
				return m_data;
			}

			void setGroupTransform(const core::matrix4& world);

			CGroup* createParticleGroup();

			CSubGroup* createParticleSubGroup(CGroup* group);

			inline u32 getNumOfGroup()
			{
				return m_data->Groups.size();
			}

			inline CGroup* getGroup(int i)
			{
				return m_data->Groups[i];
			}

			void removeParticleGroup(CGroup* group);

			void updateParticleCountByPercentage(float f, bool includeSubGroup = false);

			void Play();

			void Stop();

			bool IsPlaying();

			u32 getTotalParticle();

			inline const char* getSourcePath()
			{
				return m_sourcePath.c_str();
			}

			inline void setSourcePath(const char* path)
			{
				m_sourcePath = path;
			}

			bool load();

			bool save();

			CGroup* duplicateGroup(CGroup* group);

			CEmitter* duplicateEmitter(CGroup* group, CEmitter* emitter);

			DECLARE_GETTYPENAME(CParticleComponent)

		protected:

			void saveGroups(CObjectSerializable* groups);

			void saveGroup(Particle::CGroup* group, CObjectSerializable* object);

			void loadGroup(Particle::CGroup* group, io::IXMLReader* reader);

			void loadGroup(Particle::CGroup* group, CObjectSerializable* object);

			void loadEmitters(Particle::CGroup* group, io::IXMLReader* reader);

			void loadModels(Particle::CGroup* group, io::IXMLReader* reader);

			void loadRenderer(Particle::CGroup* group, io::IXMLReader* reader);

			void loadSubGroups(Particle::CGroup* group, io::IXMLReader* reader);
		};
	}
}