/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "Reactive/CSubject.h"
#include "Editor/Gizmos/CGizmos.h"

#include "ParticleSystem/Particles/Emitters/CDirectionEmitter.h"
#include "ParticleSystem/Particles/Zones/CPositionZone.h"

namespace Skylicht
{
	namespace Editor
	{
		class CParticleGizmos :
			public CGizmos,
			public IObserver
		{
		public:
			enum EState
			{
				None = 0,
				Emitter,
				Zone,
				Gravity,
				Orientation
			};

		protected:
			core::matrix4 m_parentWorld;

			CSubject<core::vector3df> m_position;
			CSubject<core::quaternion> m_rotation;

			int m_selectPointId;

			bool m_changed;

			EState m_state;

			Particle::CGroup* m_group;
			Particle::CDirectionEmitter* m_emitter;
			Particle::CZone* m_zone;
			Particle::CPositionZone* m_positionZone;

		public:
			CParticleGizmos();

			virtual ~CParticleGizmos();

			void setGravity(Particle::CGroup* group, const core::matrix4& world);

			void setOrientation(Particle::CGroup* group, const core::matrix4& world);

			void setEmitter(Particle::CDirectionEmitter* emitter, const core::matrix4& world);

			void setZone(Particle::CZone* zone, const core::matrix4& world);

			virtual void onGizmos();

			virtual void onEnable();

			virtual void onRemove();

			virtual void refresh();

			void reset();

			virtual void onNotify(ISubject* subject, IObserver* from);

			CSubject<core::vector3df>& getPosition()
			{
				return m_position;
			}

			CSubject<core::quaternion>& getRotation()
			{
				return m_rotation;
			}

			void setPosition(const core::vector3df& pos);

			void setRotation(const core::quaternion& rot);

			inline EState getState()
			{
				return m_state;
			}

			inline Particle::CGroup* getGroup()
			{
				return m_group;
			}

			inline Particle::CDirectionEmitter* getDirectionEmitter()
			{
				return m_emitter;
			}

			inline Particle::CZone* getZone()
			{
				return m_zone;
			}

		protected:

			void updateProperty();
		};
	}
}