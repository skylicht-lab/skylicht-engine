/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#include "Scene/CScene.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSelecting :public IEventReceiver
		{
		public:
			DECLARE_SINGLETON(CSelecting)

		protected:
			core::triangle3df m_triangles[12];

			bool m_altPressed;

			bool m_leftMousePressed;
			bool m_allowDragSelect;

			core::vector2di m_mouseBegin;
			core::vector2di m_mousePosition;
		public:
			CSelecting();

			virtual ~CSelecting();

			void end();

			virtual bool OnEvent(const SEvent& event);

			void getObjectWithRay(const core::line3d<f32>& ray, f32& outBestDistanceSquared, CGameObject*& object, CEntity*& entity);

			void getObjectInRect(const core::rectf& r, std::vector<CGameObject*>& objects, std::vector<CEntity*>& entities);

			bool isDragSelect(core::vector2di& from, core::vector2di& to);

			bool isDragSelect();

		protected:

			void doSingleSelect();

			void doMultiSelect();

			void doSelect(CGameObject* object, CEntity* entity);

			void addSelect(CGameObject* object, CEntity* entity);

			bool project3Dto2D(const core::vector3df& pos, core::matrix4& transform, core::dimension2df& dim, core::vector2df& out);
		};
	}
}