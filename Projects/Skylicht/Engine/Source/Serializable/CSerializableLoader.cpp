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

#include "pch.h"
#include "CSerializableLoader.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{

	void CSerializableLoader::load(io::IXMLReader* reader, CObjectSerializable* object, const char* exitNode)
	{
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IAttributes* attr = fs->createEmptyAttributes();

		std::wstring nodeName = L"node";
		std::wstring attributeName = CStringImp::convertUTF8ToUnicode(object->Name.c_str());

		if (nodeName == reader->getNodeName() && attributeName == reader->getAttributeValue(L"type"))
		{
			attr->read(reader);

			if (object->getNumProperty() > 0)
				object->deserialize(attr);	// for SerializableActivator
			else
			{
				if (object->isArray())
				{
					CArraySerializable* arrayObject = dynamic_cast<CArraySerializable*>(object);
					if (arrayObject->haveCreateElementFunction())
					{
						// create element and deserialize
						int numElement = attr->getAttributeCount();
						arrayObject->resize(numElement);
						arrayObject->deserialize(attr);
					}
					else
					{
						// this is array but no create element function
						initProperty(object, attr);
					}
				}
				else
				{
					initProperty(object, attr);
				}
			}
		}
		else
		{
			char log[512];
			sprintf(log, "[CObjectSerializable::load] Skip wrong data: type: %s", object->Name.c_str());
			os::Printer::log(log);
			attr->drop();
			return;
		}
		attr->drop();

		// continue read
		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				// <node>
				if (nodeName == reader->getNodeName())
				{
					std::wstring type = reader->getAttributeValue(L"type");
					std::string name = CStringImp::convertUnicodeToUTF8(type.c_str());

					const wchar_t* isArray = reader->getAttributeValue(L"array");

					bool newObject = true;
					CObjectSerializable* data;

					// activator
					data = CSerializableActivator::getInstance()->createInstance(name.c_str());
					if (data == NULL)
					{
						// try find the current object with the name
						data = dynamic_cast<CObjectSerializable*>(object->getProperty(name.c_str()));
						if (data != NULL)
						{
							// use exist property
							newObject = false;
						}
						else
						{
							// we will add new object serializable
							if (isArray)
								data = new CArraySerializable(name.c_str());
							else
								data = new CObjectSerializable(name.c_str());

							newObject = true;
						}
					}

					load(reader, data, exitNode);

					if (newObject)
					{
						object->addProperty(data);
						object->autoRelease(data);
					}

					// if end the node
					// </node> // <node type="Components">
					if (name == exitNode)
						return;
				}
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				// </node>
				if (nodeName == reader->getNodeName())
				{
					return;
				}
			}
			break;
			default:
				break;
			}
		}
	}

	void CSerializableLoader::loadSerializable(const char* file, CObjectSerializable* object)
	{
		io::IXMLReader* reader = getIrrlichtDevice()->getFileSystem()->createXMLReader(file);
		if (reader == NULL)
			return;

		object->setSavePath(file);

		std::wstring nodeName = L"node";

		while (reader->read())
		{
			switch (reader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				// <node>
				if (nodeName == reader->getNodeName())
				{
					load(reader, object, "~");	// todo load to end the file
				}
			}
			break;
			default:
				break;
			}
		}

		reader->drop();
	}

	void CSerializableLoader::initProperty(CObjectSerializable* object, io::IAttributes* attributes)
	{
		for (u32 i = 0, n = attributes->getAttributeCount(); i < n; i++)
		{
			io::E_ATTRIBUTE_TYPE type = attributes->getAttributeType(i);
			const c8* name = attributes->getAttributeName(i);

			CValueProperty* valueProperty = NULL;

			switch (type)
			{
			case io::EAT_INT:
				valueProperty = new CIntProperty(object, name, attributes->getAttributeAsInt(i));
				break;
			case io::EAT_UINT:
				valueProperty = new CUIntProperty(object, name, attributes->getAttributeAsUInt(i));
				break;
			case io::EAT_FLOAT:
				valueProperty = new CFloatProperty(object, name, attributes->getAttributeAsFloat(i));
				break;
			case io::EAT_BOOL:
				valueProperty = new CBoolProperty(object, name, attributes->getAttributeAsBool(i));
				break;
			case io::EAT_STRING:
				valueProperty = new CStringProperty(object, name, attributes->getAttributeAsString(i).c_str());
				break;
			case io::EAT_VECTOR3D:
				valueProperty = new CVector3Property(object, name, attributes->getAttributeAsVector3d(i));
				break;
			case io::EAT_QUATERNION:
				valueProperty = new CQuaternionProperty(object, name, attributes->getAttributeAsQuaternion(i));
				break;
			case io::EAT_COLOR:
				valueProperty = new CColorProperty(object, name, attributes->getAttributeAsColor(i));
				break;
			case io::EAT_MATRIX:
				valueProperty = new CMatrixProperty(object, name, attributes->getAttributeAsMatrix(i));
				break;
			};

			if (valueProperty)
				object->autoRelease(valueProperty);
		}
	}
}