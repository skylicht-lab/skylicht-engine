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

#include "CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		class COMPONENT_API CSubGroup :
			public CGroup,
			public IParticleCallback
		{
		protected:
			CGroup* m_parentGroup;

			ISystem* m_parentSystem;

			core::vector3df m_position;
			core::vector3df m_direction;
			core::quaternion m_rotate;

			bool m_followParentTransform;
			bool m_emitterWorldOrientation;
			bool m_syncLife;
			bool m_syncColor;

		public:
			CSubGroup(CGroup* group);

			virtual ~CSubGroup();

			void initParticles();

			virtual void OnParticleBorn(CParticle& p);

			virtual void OnParticleDead(CParticle& p);

			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2);

			virtual void OnGroupDestroy();

			virtual void updateLaunchEmitter();

			virtual void bornParticle();

			virtual core::vector3df getTransformPosition(const core::vector3df& pos);

			virtual core::vector3df getTransformVector(const core::vector3df& vec);

			inline void setFollowParentTransform(bool b)
			{
				m_followParentTransform = b;
				m_parentSystem->setEnable(b);
			}

			inline void setEmitterWorldOrientation(bool b)
			{
				m_emitterWorldOrientation = b;
			}

			void syncParentParams(bool life, bool color);

			inline CGroup* getParentGroup()
			{
				return m_parentGroup;
			}

			DECLARE_GETTYPENAME(CSubGroup)

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);
		};
	}
}