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

#pragma once

#include "CEntity.h"

namespace Skylicht
{
	class CEntityManager;

	/// @brief This is the interface for object classes responsible for processing Entity data.
	/// @ingroup ECS
	/// 
	/// Code example for adding a system
	/// @code
	/// CEntityManager* entityManager = scene->getEntityManager();
	/// entityManager->addSystem<CBoidSystem>();
	/// @endcode
	/// 
	/// A IEntitySystem will operate by querying entities that have data requiring processing (onQuery), and then running an update function to process the entity's data.
	/// 
	/// @see CEntityGroup
	class SKYLICHT_API IEntitySystem
	{
	protected:
		int m_systemOrder;

	public:
		IEntitySystem() :
			m_systemOrder(0)
		{
		}

		virtual ~IEntitySystem()
		{

		}

		virtual bool isRenderSystem()
		{
			return false;
		}

		virtual void beginQuery(CEntityManager* entityManager) = 0;

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int count) = 0;

		virtual void init(CEntityManager* entityManager) = 0;

		virtual void update(CEntityManager* entityManager) = 0;

		inline void setSystemOrder(int order)
		{
			m_systemOrder = order;
		}

		inline int getSystemOrder()
		{
			return m_systemOrder;
		}
	};
}