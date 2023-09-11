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
		CIntProperty() :
			CIntProperty(NULL, "CIntProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CIntProperty* value = new CIntProperty(NULL, Name.c_str());
			value->m_value = m_value;
			value->Min = Min;
			value->Max = Max;
			value->ClampMin = ClampMin;
			value->ClampMax = ClampMax;
			return value;
		}
	};

	class CUIntProperty : public CValuePropertyTemplate<u32>
	{
	public:
		u32 Max;
		bool ClampMax;

	public:
		CUIntProperty() :
			CUIntProperty(NULL, "CUIntProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CUIntProperty* value = new CUIntProperty(NULL, Name.c_str());
			value->m_value = m_value;
			value->Max = Max;
			value->ClampMax = ClampMax;
			return value;
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
		CFloatProperty() :
			CFloatProperty(NULL, "CFloatProperty")
		{
		}

		CFloatProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(false),
			ClampMax(false),
			Min(-FLT_MAX),
			Max(FLT_MAX)
		{
			set(0.0f);
		}

		CFloatProperty(CObjectSerializable* owner, const char* name, float value) :
			CValuePropertyTemplate(owner, Float, name),
			ClampMin(false),
			ClampMax(false),
			Min(-FLT_MAX),
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

		virtual CValueProperty* clone()
		{
			CFloatProperty* value = new CFloatProperty(NULL, Name.c_str());
			value->m_value = m_value;
			value->Min = Min;
			value->Max = Max;
			value->ClampMin = ClampMin;
			value->ClampMax = ClampMax;
			return value;
		}
	};

	class CStringProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CStringProperty() :
			CStringProperty(NULL, "CStringProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CStringProperty* value = new CStringProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CFolderPathProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		CFolderPathProperty() :
			CFolderPathProperty(NULL, "CFolderPathProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CFolderPathProperty* value = new CFolderPathProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CFilePathProperty : public CValuePropertyTemplate<std::string>
	{
	public:
		std::vector<std::string> Exts;

	public:
		CFilePathProperty() :
			CFilePathProperty(NULL, "CFilePathProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CFilePathProperty* value = new CFilePathProperty(NULL, Name.c_str());
			value->m_value = m_value;
			value->Exts = Exts;
			return value;
		}
	};

	class CImageProperty : public CValuePropertyTemplate<std::string>
	{
	public:

	public:
		CImageProperty() :
			CImageProperty(NULL, "CImageProperty")
		{
		}

		CImageProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Image, name)
		{
		}

		CImageProperty(CObjectSerializable* owner, const char* name, const char* value) :
			CValuePropertyTemplate(owner, Image, name)
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

		virtual CValueProperty* clone()
		{
			CImageProperty* value = new CImageProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CSpriteProperty : public CValuePropertyTemplate<std::string>
	{
	public:

	public:
		CSpriteProperty() :
			CSpriteProperty(NULL, "CSpriteProperty")
		{
		}

		CSpriteProperty(CObjectSerializable* owner, const char* name) :
			CValuePropertyTemplate(owner, Sprite, name)
		{
		}

		CSpriteProperty(CObjectSerializable* owner, const char* name, const char* value) :
			CValuePropertyTemplate(owner, Sprite, name)
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

		virtual CValueProperty* clone()
		{
			CSpriteProperty* value = new CSpriteProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CBoolProperty : public CValuePropertyTemplate<bool>
	{
	public:
		CBoolProperty() :
			CBoolProperty(NULL, "CBoolProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CBoolProperty* value = new CBoolProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CDateTimeProperty : public CValuePropertyTemplate<long>
	{
	public:
		CDateTimeProperty() :
			CDateTimeProperty(NULL, "CDateTimeProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CDateTimeProperty* value = new CDateTimeProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CVector3Property : public CValuePropertyTemplate<core::vector3df>
	{
	public:
		CVector3Property() :
			CVector3Property(NULL, "CVector3Property")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CVector3Property* value = new CVector3Property(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CQuaternionProperty : public CValuePropertyTemplate<core::quaternion>
	{
	public:
		CQuaternionProperty() :
			CQuaternionProperty(NULL, "CVector3Property")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CQuaternionProperty* value = new CQuaternionProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CColorProperty : public CValuePropertyTemplate<video::SColor>
	{
	public:
		CColorProperty() :
			CColorProperty(NULL, "CColorProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CColorProperty* value = new CColorProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
		}
	};

	class CMatrixProperty : public CValuePropertyTemplate<core::matrix4>
	{
	public:
		CMatrixProperty() :
			CMatrixProperty(NULL, "CMatrixProperty")
		{
		}

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

		virtual CValueProperty* clone()
		{
			CMatrixProperty* value = new CMatrixProperty(NULL, Name.c_str());
			value->m_value = m_value;
			return value;
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

		SEnumString* getEnumByValue(int value)
		{
			for (int i = 0, n = (int)m_enums.size(); i < n; i++)
			{
				if (m_enums[i].Value == value)
					return &m_enums[i];
			}
			return NULL;
		}

		const std::vector<SEnumString>& getEnum()
		{
			return m_enums;
		}

		void setEnums(const std::vector<SEnumString>& e)
		{
			m_enums = e;
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
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			obj->set(value);
		}

		void addEnumString(const char* name, T value)
		{
			m_enums.push_back(SEnumString(name, static_cast<int>(value)));
		}

		virtual void setIntValue(int value)
		{
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			obj->set(static_cast<T>(value));
		}

		virtual int getIntValue()
		{
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			return static_cast<int>(obj->get());
		}

		virtual void serialize(io::IAttributes* io)
		{
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			io->addInt(obj->Name.c_str(), static_cast<int>(obj->get()));
		}

		virtual void deserialize(io::IAttributes* io)
		{
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			obj->set(static_cast<T>(io->getAttributeAsInt(obj->Name.c_str())));
		}

		virtual CValueProperty* clone()
		{
			CValuePropertyTemplate<T>* obj = dynamic_cast<CValuePropertyTemplate<T>*>(this);
			CEnumPropertyData* enumObj = dynamic_cast<CEnumPropertyData*>(this);

			CEnumProperty<T>* value = new CEnumProperty<T>(NULL, obj->Name.c_str(), obj->get());
			CEnumPropertyData* valueEnum = dynamic_cast<CEnumPropertyData*>(value);
			valueEnum->setEnums(enumObj->getEnum());

			return value;
		}
	};
}