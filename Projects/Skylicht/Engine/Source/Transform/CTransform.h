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

	class CTransform : public CComponentSystem
	{
	public:
		static core::vector3df s_ox;
		static core::vector3df s_oy;
		static core::vector3df s_oz;

	protected:
		bool m_hasChanged;

	public:
		CTransform();

		virtual ~CTransform();

		virtual void initComponent();

		virtual void updateComponent();

		void setChanged(bool b)
		{
			m_hasChanged = b;
		}

		bool hasChanged()
		{
			return m_hasChanged;
		}

		CTransform* getParent();

		CGameObject* getParentObject();

		CEntity* getParentEntity();

	public:

		virtual void setRelativeTransform(const core::matrix4& mat) = 0;

		virtual const core::matrix4& getRelativeTransform() = 0;

		virtual void getRelativeTransform(core::matrix4& matrix) = 0;

		DECLARE_GETTYPENAME(CTransform)
	};
}