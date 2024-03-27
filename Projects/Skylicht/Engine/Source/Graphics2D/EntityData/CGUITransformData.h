/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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

#include "Entity/IEntityData.h"

namespace Skylicht
{
	class CGUITransformData : public IEntityData
	{
		friend class CGUILayoutSystem;

	protected:
		core::vector3df m_transformPosition;

		core::vector3df	m_position;
		core::vector3df	m_scale;
		core::vector3df	m_rotation;

	public:
		bool HasChanged;

		core::rectf Rect;

		CGUITransformData* Parent;

	public:
		CGUITransformData();

		virtual ~CGUITransformData();

		inline const core::vector3df& getPosition()
		{
			return m_position;
		}

		inline const core::vector3df& getAlignPosition()
		{
			return m_transformPosition;
		}

		inline void setPosition(const core::vector3df& v)
		{
			m_position = v;
			HasChanged = true;
		}

		inline const core::vector3df& getScale()
		{
			return m_scale;
		}

		inline void setScale(const core::vector3df& v)
		{
			m_scale = v;
			HasChanged = true;
		}

		inline const core::vector3df& getRotation()
		{
			return m_rotation;
		}

		inline void setRotation(const core::vector3df& v)
		{
			m_rotation = v;
			HasChanged = true;
		}

		inline float getHeight()
		{
			return Rect.getHeight();
		}

		inline float getWidth()
		{
			return Rect.getWidth();
		}

		inline const core::rectf& getRect()
		{
			return Rect;
		}

		inline void setRect(const core::rectf& r)
		{
			Rect = r;
		}

		DECLARE_GETTYPENAME(CGUITransformData);
	};

	DECLARE_DATA_TYPE_INDEX(CGUITransformData);
}