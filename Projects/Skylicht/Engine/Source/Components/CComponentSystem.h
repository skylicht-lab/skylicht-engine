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

#include "Utils/CActivator.h"
#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	class CGameObject;

	class CComponentSystem : public IActivatorObject
	{
	protected:
		CGameObject* m_gameObject;

		void setOwner(CGameObject* obj)
		{
			m_gameObject = obj;
		}

		bool m_enable;

	public:
		friend class CGameObject;

		CComponentSystem();

		virtual ~CComponentSystem();

		const char* getName();

		virtual void initComponent() = 0;

		virtual void startComponent();

		virtual void updateComponent() = 0;

		virtual void postUpdateComponent();

		virtual void endUpdate();

		virtual void onEnable(bool b);

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		void setEnable(bool b);

		inline bool isEnable()
		{
			return m_enable;
		}

		inline CGameObject* getGameObject()
		{
			return m_gameObject;
		}

		DECLARE_GETTYPENAME(CComponentSystem)
	};
}