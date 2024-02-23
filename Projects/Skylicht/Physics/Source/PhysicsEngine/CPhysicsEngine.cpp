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
#include "CPhysicsEngine.h"

namespace Skylicht
{
	namespace Physics
	{
		CPhysicsEngine::CPhysicsEngine() :
			m_gravity(-10.0f),
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_broadphase(NULL),
			m_dispatcher(NULL),
			m_solver(NULL),
			m_collisionConfiguration(NULL),
			m_dynamicsWorld(NULL)
#endif
		{

		}

		CPhysicsEngine::~CPhysicsEngine()
		{
			exitPhysics();
		}

		void CPhysicsEngine::initPhysics()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			// collision configuration contains default setup for memory, collision setup. 
			// Advanced users can create their own configuration.
			m_collisionConfiguration = new btDefaultCollisionConfiguration();

			// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher 
			// (see Extras/BulletMultiThreaded)
			m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

			// btDbvtBroadphase is a good general purpose broadphase. 
			// You can also try out btAxis3Sweep.
			m_broadphase = new btDbvtBroadphase();

			// the default constraint solver. 
			// For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			m_solver = new btSequentialImpulseConstraintSolver();

			m_dynamicsWorld = new btDiscreteDynamicsWorld(
				m_dispatcher,
				m_broadphase,
				m_solver,
				m_collisionConfiguration);

			m_dynamicsWorld->setGravity(btVector3(0.0f, m_gravity, 0.0f));
#endif
		}

		void CPhysicsEngine::exitPhysics()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				delete m_dynamicsWorld;
				delete m_solver;
				delete m_broadphase;
				delete m_dispatcher;
				delete m_collisionConfiguration;

				m_dynamicsWorld = NULL;
				m_solver = NULL;
				m_broadphase = NULL;
				m_dispatcher = NULL;
				m_collisionConfiguration = NULL;
			}
#endif
		}

		void CPhysicsEngine::updatePhysics(float timestepSec)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				m_dynamicsWorld->stepSimulation(timestepSec);
			}
#endif
		}

		void CPhysicsEngine::setGravity(float g)
		{
			m_gravity = g;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				m_dynamicsWorld->setGravity(btVector3(0.0f, m_gravity, 0.0f));
			}
#endif
		}
	}
}