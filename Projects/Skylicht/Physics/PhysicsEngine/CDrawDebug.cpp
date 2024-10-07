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

#include "pch.h"
#include "CDrawDebug.h"
#include "Debug/CSceneDebug.h"
#include "RigidBody/CRigidbody.h"

#ifdef USE_BULLET_PHYSIC_ENGINE

namespace Skylicht
{
	namespace Physics
	{
		CDrawDebug::CDrawDebug() :
			m_debugMode(0),
			m_enableDraw(false)
		{
			m_debugMode |= btIDebugDraw::DBG_DrawWireframe;
			m_debugMode |= btIDebugDraw::DBG_DrawConstraints;
			m_debugMode |= btIDebugDraw::DBG_DrawConstraintLimits;
		}

		CDrawDebug::~CDrawDebug()
		{

		}

		void CDrawDebug::drawCurrentObject(btCollisionObject* obj)
		{
			if (obj->getUserPointer())
			{
				CRigidbody* rigidBody = (CRigidbody*)obj->getUserPointer();
				m_enableDraw = rigidBody->enableDrawDebug();
			}
			else
			{
				m_enableDraw = false;
			}
		}

		void CDrawDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
		{
			if (!m_enableDraw)
				return;

			CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();
			debug->addLine(
				core::vector3df(from.x(), from.y(), from.z()),
				core::vector3df(to.x(), to.y(), to.z()),
				SColor(255, 200, 0, 0)
			);
		}

		void CDrawDebug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
		{

		}

		void CDrawDebug::reportErrorWarning(const char* warningString)
		{

		}

		void CDrawDebug::draw3dText(const btVector3& location, const char* textString)
		{

		}

		void CDrawDebug::setDebugMode(int debugMode)
		{
			m_debugMode = debugMode;
		}
	}
}

#endif