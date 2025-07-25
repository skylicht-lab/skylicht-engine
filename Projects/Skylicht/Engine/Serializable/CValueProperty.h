/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include <functional>

namespace Skylicht
{
	class CObjectSerializable;

	enum EPropertyDataType
	{
		String,
		StringW,
		Integer,
		UInteger,
		Float,
		Decimal,
		DateTime,
		Bool,
		Vector3,
		Vector2,
		Quaternion,
		Color,
		Matrix4,
		Object,
		FilePath,
		FolderPath,
		ImageSource,
		FrameSource,
		Enum,
		NumType
	};

	enum EPropertyObjectType
	{
		None,
		ObjectArray,
		ObjectInterpolate,
		FileArray,
		TextureArray,
		NumObjectType
	};

	class SKYLICHT_API CValueProperty
	{
	protected:
		EPropertyDataType m_dataType;
		EPropertyObjectType m_objectType;

		CObjectSerializable* m_owner;

		// ui editor interface
		std::string m_uiHeader;

		float m_uiSpace;

		bool m_hidden;

	public:
		std::function<void(bool)> OnSetHidden;
		std::function<void()> OnChanged;

	public:
		std::string Name;
		std::vector<std::string> OtherName;

		CValueProperty(CObjectSerializable* owner, EPropertyDataType dataType, const char* name);

		virtual ~CValueProperty();

		void setOwner(CObjectSerializable* owner)
		{
			m_owner = owner;
		}

		EPropertyDataType getType()
		{
			return m_dataType;
		}

		EPropertyObjectType getObjectType()
		{
			return m_objectType;
		}

		virtual void serialize(io::IAttributes* io) = 0;

		virtual void deserialize(io::IAttributes* io) = 0;

		virtual CValueProperty* clone() = 0;

		inline void setUIHeader(const char* header)
		{
			m_uiHeader = header;
		}

		inline const std::string& getUIHeader()
		{
			return m_uiHeader;
		}

		inline void setUISpace(float space)
		{
			m_uiSpace = space;
		}

		inline float getUISpace()
		{
			return m_uiSpace;
		}

		inline void setHidden(bool b)
		{
			m_hidden = b;
			if (OnSetHidden != nullptr)
				OnSetHidden(b);
		}

		inline bool isHidden()
		{
			return m_hidden;
		}
	};
}