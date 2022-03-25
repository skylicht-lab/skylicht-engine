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

#include "CValueProperty.h"

namespace Skylicht
{
	template<class T>
	class CValuePropertyTemplate : public CValueProperty
	{
	protected:
		T m_value;

	public:
		CValuePropertyTemplate(CObjectSerializable* owner, EPropertyDataType dataType, const char* name) :
			CValueProperty(owner, dataType, name)
		{
		}

		virtual ~CValuePropertyTemplate()
		{

		}

		inline void set(const T& v)
		{
			m_value = v;
		}

		inline const T& get()
		{
			return m_value;
		}

		T& operator*()
		{
			return m_value;
		}
	};

	class CIntProperty : public CValuePropertyTemplate<int>
	{
	public:
		int Min;
		int Max;
		bool ClampMin;
		bool ClampMax;

	public:
		CIntProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Integer, name),
			Min(INT_MIN),
			Max(INT_MAX),
			ClampMin(false),
			ClampMax(false)
		{
			set(0);
		}

		CIntProperty(CObjectSerializable* owner, const char* name, int value) :
			CValuePropertyTemplate(owner, Integer, name),
			Min(INT_MIN),
			Max(INT_MAX),
			ClampMin(false),
			ClampMax(false)
		{
			set(value);
		}

		CIntProperty(CObjectSerializable* owner, const char* name, int value, int min) :
			CValuePropertyTemplate(owner, Integer, name),
			Min(min),
			Max(INT_MAX),
			ClampMin(true),
			ClampMax(false)
		{
			set(value);
		}

		CIntProperty(CObjectSerializable* owner, const char* name, int value, int min, int max) :
			CValuePropertyTemplate(owner, Integer, name),
			Min(min),
			Max(max),
			ClampMin(true),
			ClampMax(true)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addInt(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsInt(Name.c_str());
		}
	};

	class CUIntProperty : public CValuePropertyTemplate<u32>
	{
	public:
		u32 Max;
		bool ClampMax;

	public:
		CUIntProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, UInteger, name),
			Max(UINT_MAX),
			ClampMax(false)
		{
			set(0);
		}

		CUIntProperty(CObjectSerializable* owner, const char* name, u32 value) :
			CValuePropertyTemplate(owner, UInteger, name),
			Max(UINT_MAX),
			ClampMax(false)
		{
			set(value);
		}

		CUIntProperty(CObjectSerializable* owner, const char* name, u32 value, u32 max) :
			CValuePropertyTemplate(owner, UInteger, name),
			Max(max),
			ClampMax(true)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addInt(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsInt(Name.c_str());
		}
	};

	class CFloatProperty : public CValuePropertyTemplate<float>
	{
	public:
		float Min;
		float Max;
		bool ClampMin;
		bool ClampMax;

	public:
		CFloatProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(false),
			ClampMax(false),
			Min(FLT_MIN),
			Max(FLT_MAX)
		{
			set(0.0f);
		}

		CFloatProperty(CObjectSerializable* owner, const char* name, float value) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(false),
			ClampMax(false),
			Min(FLT_MIN),
			Max(FLT_MAX)
		{
			set(value);
		}

		CFloatProperty(CObjectSerializable* owner, const char* name, float value, float min, float max) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(true),
			ClampMax(true),
			Min(min),
			Max(max)
		{
			set(value);
		}

		CFloatProperty(CObjectSerializable* owner, const char* name, float value, float min) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(true),
			ClampMax(false),
			Min(min),
			Max(FLT_MAX)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addFloat(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsFloat(Name.c_str());
		}
	};

	class CStringProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CStringProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, String, name)
		{
		}

		CStringProperty(CObjectSerializable* owner, const char* name, const char* value) :
			CValuePropertyTemplate(owner, String, name)
		{
			set(value);
		}

		const char* getString()
		{
			return m_value.c_str();
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addString(Name.c_str(), m_value.c_str());
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsString(Name.c_str()).c_str();
		}
	};

	class CFolderPathProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CFolderPathProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, FolderPath, name)
		{
		}

		CFolderPathProperty(CObjectSerializable* owner, const char* name, const char* value) :
			CValuePropertyTemplate(owner, FolderPath, name)
		{
			set(value);
		}

		const char* getString()
		{
			return m_value.c_str();
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addString(Name.c_str(), m_value.c_str());
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsString(Name.c_str()).c_str();
		}
	};

	class CFilePathProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		std::vector<std::string> Exts;

	public:
		CFilePathProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, FilePath, name)
		{
		}

		CFilePathProperty(CObjectSerializable* owner, const char* name, const char* value, const std::vector<std::string>& exts) :
			CValuePropertyTemplate(owner, FilePath, name)
		{
			Exts = exts;
			set(value);
		}

		CFilePathProperty(CObjectSerializable* owner, const char* name, const char* value, const char* ext) :
			CValuePropertyTemplate(owner, FilePath, name)
		{
			Exts.push_back(ext);
			set(value);
		}

		const char* getString()
		{
			return m_value.c_str();
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addString(Name.c_str(), m_value.c_str());
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsString(Name.c_str()).c_str();
		}
	};

	class CBoolProperty : public CValuePropertyTemplate<bool>
	{
	public:
		CBoolProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Bool, name)
		{
		}

		CBoolProperty(CObjectSerializable* owner, const char* name, bool value) :
			CValuePropertyTemplate(owner, Bool, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addBool(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsBool(Name.c_str());
		}
	};

	class CStringWProperty : public CValuePropertyTemplate<std::wstring>
	{
	public:
		CStringWProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, StringW, name)
		{
		}

		CStringWProperty(CObjectSerializable* owner, const char* name, const wchar_t* value) :
			CValuePropertyTemplate(owner, StringW, name)
		{
			set(value);
		}

		const wchar_t* getString()
		{
			return m_value.c_str();
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addString(Name.c_str(), m_value.c_str());
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsStringW(Name.c_str()).c_str();
		}
	};

	class CDateTimeProperty : public CValuePropertyTemplate<long>
	{
	public:
		CDateTimeProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
			set(0);
		}

		CDateTimeProperty(CObjectSerializable* owner, const char* name, long value) :
			CValuePropertyTemplate(owner, DateTime, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addFloat(Name.c_str(), (float)m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = (long)io->getAttributeAsFloat(Name.c_str());
		}
	};

	class CVector3Property : public CValuePropertyTemplate<core::vector3df>
	{
	public:
		CVector3Property(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Vector3, name)
		{
			set(core::vector3df());
		}

		CVector3Property(CObjectSerializable* owner, const char* name, const core::vector3df& value) :
			CValuePropertyTemplate(owner, Vector3, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addVector3d(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsVector3d(Name.c_str());
		}
	};

	class CQuaternionProperty : public CValuePropertyTemplate<core::quaternion>
	{
	public:
		CQuaternionProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Quaternion, name)
		{
			set(core::quaternion());
		}

		CQuaternionProperty(CObjectSerializable* owner, const char* name, const core::quaternion& value) :
			CValuePropertyTemplate(owner, Quaternion, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addQuaternion(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsQuaternion(Name.c_str());
		}
	};

	class CColorProperty : public CValuePropertyTemplate<video::SColor>
	{
	public:
		CColorProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Color, name)
		{
			set(SColor(255, 255, 255, 255));
		}

		CColorProperty(CObjectSerializable* owner, const char* name, const video::SColor& value) :
			CValuePropertyTemplate(owner, Color, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addColor(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsColor(Name.c_str());
		}
	};

	class CMatrixProperty : public CValuePropertyTemplate<core::matrix4>
	{
	public:
		CMatrixProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Matrix4, name)
		{
			set(core::IdentityMatrix);
		}

		CMatrixProperty(CObjectSerializable* owner, const char* name, const core::matrix4& value) :
			CValuePropertyTemplate(owner, Matrix4, name)
		{
			set(value);
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addMatrix(Name.c_str(), m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = io->getAttributeAsMatrix(Name.c_str());
		}
	};

	class CEnumPropertyData
	{
	public:
		struct SEnumString
		{
			std::string Name;
			int Value;

			SEnumString(const char* name, int value)
			{
				Name = name;
				Value = value;
			}
		};

	protected:
		std::vector<SEnumString> m_enums;

	public:

		void addEnumStringInt(const char* name, int value)
		{
			m_enums.push_back(SEnumString(name, (int)value));
		}

		int getEnumCount()
		{
			return (int)m_enums.size();
		}

		const SEnumString& getEnum(int i)
		{
			return m_enums[i];
		}

		virtual void setIntValue(int value) = 0;

		virtual int getIntValue() = 0;
	};

	template<class T>
	class CEnumProperty :
		public CValuePropertyTemplate<T>,
		public CEnumPropertyData
	{
	public:


	public:
		CEnumProperty(CObjectSerializable* owner, const char* name, T value) :
			CValuePropertyTemplate<T>(owner, Enum, name)
		{
			set(value);
		}

		void addEnumString(const char* name, T value)
		{
			m_enums.push_back(SEnumString(name, (int)value));
		}

		virtual void setIntValue(int value)
		{
			set((T)value);
		}

		virtual int getIntValue()
		{
			return (int)m_value;
		}

		virtual void serialize(io::IAttributes* io)
		{
			io->addInt(Name.c_str(), (int)m_value);
		}

		virtual void deserialize(io::IAttributes* io)
		{
			m_value = (T)io->getAttributeAsInt(Name.c_str());
		}
	};
}