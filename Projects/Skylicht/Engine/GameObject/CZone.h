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

#include "CContainerObject.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	class CScene;

	/// @brief It's the primary container object for the CScene; all CGameObject have a CZone as their root parent.
	/// @ingroup GameObject
	class SKYLICHT_API CZone : public CContainerObject
	{
	protected:
		CEntityManager* m_entityManager;
		CScene* m_scene;
		bool m_mainZoneInEditor;

	public:
		CZone(CScene* scene);
		virtual ~CZone();
		virtual void remove();
		virtual void updateObject();
		virtual void endUpdate();
		virtual CGameObject* searchObjectInScene(const wchar_t* objectName);
		virtual bool testConflictName(const wchar_t* objectName);

		DECLARE_GETTYPENAME(CZone)

	public:

		inline CEntityManager* getEntityManager()
		{
			return m_entityManager;
		}

		inline CScene* getScene()
		{
			return m_scene;
		}

		inline void setMainZoneInEditor(bool b)
		{
			m_mainZoneInEditor = b;
		}

		inline bool isMainZoneInEditor()
		{
			return m_mainZoneInEditor;
		}
	};

	typedef std::vector<CZone*> ArrayZone;
	typedef std::vector<CZone*>::iterator ArrayZoneIter;
}