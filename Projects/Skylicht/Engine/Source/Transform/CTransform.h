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

#include "Components/CComponentSystem.h"

namespace Skylicht
{
	class CEntity;

	namespace Transform
	{
		SKYLICHT_API extern core::vector3df Ox;
		SKYLICHT_API extern core::vector3df Oy;
		SKYLICHT_API extern core::vector3df Oz;
	}

	class SKYLICHT_API CTransform : public CComponentSystem
	{
	protected:
		bool m_hasChanged;
		bool m_attached;

		bool m_isWorldTransform;

	public:
		CTransform();

		virtual ~CTransform();

		virtual void initComponent();

		virtual void updateComponent();

		void attachTransform(CEntity* entity);

		inline void setChanged(bool b)
		{
			m_hasChanged = b;
		}

		inline bool hasChanged()
		{
			return m_hasChanged;
		}

		inline bool isAttached()
		{
			return m_attached;
		}

		inline bool isWorldTransform()
		{
			return m_isWorldTransform;
		}

		inline void setIsWorldTransform(bool b)
		{
			m_isWorldTransform = b;
			m_hasChanged = true;
		}

		CTransform* getParent();

		CGameObject* getParentObject();

		CEntity* getParentEntity();

		core::matrix4 calcWorldTransform();

		void setWorldMatrix(const core::matrix4& world);

	public:

		virtual void setRelativeTransform(const core::matrix4& mat) = 0;

		virtual const core::matrix4& getRelativeTransform() = 0;

		virtual void getRelativeTransform(core::matrix4& matrix) = 0;

		DECLARE_GETTYPENAME(CTransform)
	};
}