/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "Bullet/CBulletUtils.h"
#endif

namespace Skylicht
{
	namespace Physics
	{
		class CRigidbody : public CComponentSystem
		{
			friend class CPhysicsEngine;
		public:
			enum EActivationState
			{
				Activate,
				Sleep,
				Alway,
				Disable
			};

		protected:
			bool m_isDynamic;
			float m_mass;

#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* m_rigidBody;
			btCollisionShape* m_shape;
#endif

		public:
			CRigidbody();

			virtual ~CRigidbody();

			virtual void initComponent();

			virtual void updateComponent();

			void setDynamic(bool b);

			inline bool isDynamic()
			{
				return m_isDynamic;
			}

			void setMass(float m);

			inline float getMass()
			{
				return m_mass;
			}

			bool initRigidbody();

			void releaseRigidbody();

			core::vector3df getPosition();

			void setPosition(const core::vector3df& pos);

			core::vector3df getRotationEuler();

			core::quaternion getRotation();

			void setRotation(const core::vector3df& eulerDeg);

			void setRotation(const core::quaternion& q);

			void syncTransform();

			void setState(EActivationState state);

		private:

#ifdef USE_BULLET_PHYSIC_ENGINE
			inline btRigidBody* getBody()
			{
				return m_rigidBody;
			}

			inline btCollisionShape* getShape()
			{
				return m_shape;
			}
#endif

			DECLARE_GETTYPENAME(CRigidbody)
		};
	}
}