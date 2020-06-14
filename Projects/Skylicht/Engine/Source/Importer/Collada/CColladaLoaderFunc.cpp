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

#include "pch.h"
#include "CColladaLoaderFunc.h"

#include "Utils/CStringImp.h"
#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	//! changes the XML URI into an internal id
	void uriToId(std::wstring& str)
	{
		if (!str.size())
			return;

		if (str[0] == L'#')
			str.erase(str.begin());
	}


	std::wstring readId(io::IXMLReader *xmlRead)
	{
		std::wstring str = xmlRead->getAttributeValue(L"id");
		if (str.size() == 0)
			str = xmlRead->getAttributeValue(L"name");

		return str;
	}

	std::wstring readName(io::IXMLReader *xmlRead)
	{
		std::wstring str = xmlRead->getAttributeValue(L"name");
		return str;
	}

	void findNextNoneWhiteSpace(const c8** start)
	{
		const c8* p = *start;

		while (*p && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
			++p;

		*start = p;
	}

	inline f32 readFloat(const c8** p)
	{
		f32 ftmp;
		*p = core::fast_atof_move(*p, ftmp);
		return ftmp;
	}

	void readFloatsInsideElement(io::IXMLReader* reader, f32* floats, u32 count)
	{
		if (reader->isEmptyElement())
			return;

		while (reader->read())
		{
			if (reader->getNodeType() == io::EXN_TEXT)
			{
				// parse float data
				core::stringc data = reader->getNodeData();
				data.trim();
				const c8* p = &data[0];

				for (u32 i = 0; i < count; ++i)
				{
					findNextNoneWhiteSpace(&p);
					if (*p)
						floats[i] = readFloat(&p);
					else
						floats[i] = 0.0f;
				}
			}
			else if (reader->getNodeType() == io::EXN_ELEMENT_END)
				break; // end parsing text
		}
	}

	int calcNumTriangleFromPolygon(const s32 *vCount, int numPolygon)
	{
		int r = 0;
		for (int i = 0; i < numPolygon; i++)
		{
			int numVertex = vCount[i];

			int numTris = numVertex - 2;
			r += numTris;
		}
		return r;
	}

	int convertPolygonToTriangle(const s32 *sourcePolygon, int offset, s32 *targetTriangle, int &targetID, int numVertex, int numElementPerVertex)
	{
		// {1 2 3 4} => {1 2 3} {1 3 4}
		// {1 2 3 4 5} => {1 2 3} {1 3 4} {1 4 5}

		int numTris = numVertex - 2;
		int t = 1;

		for (int i = 0; i < numTris; i++)
		{
			int p0 = 0;
			int p1 = t;
			int p2 = (t + 1);
			t++;

			for (int j = 0; j < numElementPerVertex; j++)
				targetTriangle[targetID++] = sourcePolygon[offset + p0 * numElementPerVertex + j];

			for (int j = 0; j < numElementPerVertex; j++)
				targetTriangle[targetID++] = sourcePolygon[offset + p1 * numElementPerVertex + j];

			for (int j = 0; j < numElementPerVertex; j++)
				targetTriangle[targetID++] = sourcePolygon[offset + p2 * numElementPerVertex + j];
		}

		return offset + (numVertex * numElementPerVertex);
	}

	//! reads ints from inside of xml element until end of xml element
	void readIntsInsideElement(io::IXMLReader* reader, s32* ints, u32 count)
	{
		if (reader->isEmptyElement())
			return;

		while (reader->read())
		{
			if (reader->getNodeType() == io::EXN_TEXT)
			{
				// parse float data
				core::stringc data = reader->getNodeData();
				data.trim();
				const c8* p = &data[0];

				for (u32 i = 0; i < count; ++i)
				{
					findNextNoneWhiteSpace(&p);
					if (*p)
						ints[i] = (s32)readFloat(&p);
					else
						ints[i] = 0;
				}
			}
			else if (reader->getNodeType() == io::EXN_ELEMENT_END)
				break; // end parsing text
		}
	}

	//! reads ints from inside of xml element until end of xml element
	void readIntsInsideElement(io::IXMLReader* reader, std::vector<s32>& arrayInt)
	{
		if (reader->isEmptyElement())
			return;

		while (reader->read())
		{
			if (reader->getNodeType() == io::EXN_TEXT)
			{
				core::stringc data = reader->getNodeData();
				data.trim();
				int len = data.size();

				char* p = &data[0];
				char* begin = &data[0];

				int value = 0;

				while (*p && len > 0)
				{
					while (*p && !(*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
					{
						++p;
						len--;
					}

					*p = 0;

					if (*begin)
					{
						sscanf(begin, "%d", &value);
						arrayInt.push_back(value);
					}

					p++;
					len--;
					begin = p;
				}

			}
			else
				if (reader->getNodeType() == io::EXN_ELEMENT_END)
					break; // end parsing text
		}
	}

	void readStringInsideElement(io::IXMLReader* reader, std::wstring& string)
	{
		if (reader->isEmptyElement())
			return;

		while (reader->read())
		{
			// TODO: check for comments inside the element
			// and ignore them.

			if (reader->getNodeType() == io::EXN_TEXT)
			{
				core::stringw data = reader->getNodeData();
				data.trim();

				// result
				string = data.c_str();
			}
			else
			{
				if (reader->getNodeType() == io::EXN_ELEMENT_END)
					break; // end parsing text
			}
		}
	}

	video::SColorf readColorNode(io::IXMLReader* reader)
	{
		const core::stringc colorNodeName = "color";

		if (reader->getNodeType() == io::EXN_ELEMENT && colorNodeName == reader->getNodeName())
		{
			f32 color[4];
			readFloatsInsideElement(reader, color, 4);
			return video::SColorf(color[0], color[1], color[2], color[3]);
		}

		return video::SColorf();
	}

	f32 readFloatNode(io::IXMLReader* reader)
	{
		const core::stringc floatNodeName = "float";

		f32 result = 0.0f;
		if (reader->getNodeType() == io::EXN_ELEMENT && floatNodeName == reader->getNodeName())
		{
			readFloatsInsideElement(reader, &result, 1);
		}

		return result;
	}

	//! reads a <scale> element and its content and creates a matrix from it
	core::matrix4 readScaleNode(io::IXMLReader* reader, bool flip)
	{
		core::matrix4 mat;
		if (reader->isEmptyElement())
			return mat;

		f32 floats[3];
		readFloatsInsideElement(reader, floats, 3);

		if (flip)
			mat.setScale(core::vector3df(floats[0], floats[2], floats[1]));
		else
			mat.setScale(core::vector3df(floats[0], floats[1], floats[2]));

		return mat;
	}

	//! reads a <translate> element and its content and creates a matrix from it
	core::matrix4 readTranslateNode(io::IXMLReader* reader, bool flip)
	{
		core::matrix4 mat;
		if (reader->isEmptyElement())
			return mat;

		f32 floats[3];
		readFloatsInsideElement(reader, floats, 3);

		if (flip)
			mat.setTranslation(core::vector3df(floats[0], floats[2], floats[1]));
		else
			mat.setTranslation(core::vector3df(floats[0], floats[1], floats[2]));

		return mat;
	}

	//! reads a <rotate> element and its content and creates a matrix from it
	core::matrix4 readRotateNode(io::IXMLReader* reader, bool flip)
	{
		core::matrix4 mat;
		if (reader->isEmptyElement())
			return mat;

		f32 floats[4];
		readFloatsInsideElement(reader, floats, 4);

		if (floats[3] == -180.0f)
			floats[3] = -180.0f + 0.1f;
		else if (floats[3] == 180.0f)
			floats[3] = 180.0f - 0.1f;

		if (!core::iszero(floats[3]))
		{
			core::quaternion q;

			if (flip)
				q.fromAngleAxis(-floats[3] * core::DEGTORAD, core::vector3df(floats[0], floats[2], floats[1]));
			else
				q.fromAngleAxis(floats[3] * core::DEGTORAD, core::vector3df(floats[0], floats[1], floats[2]));

			q.normalize();
			return q.getMatrix();
		}
		else
			return core::IdentityMatrix;
	}

	core::matrix4 readMatrixNode(io::IXMLReader* reader, bool flip)
	{
		core::matrix4 mat;
		if (reader->isEmptyElement())
			return mat;

		readFloatsInsideElement(reader, mat.pointer(), 16);
		// put translation into the correct place
		if (flip)
		{
			core::matrix4 mat2(mat, core::matrix4::EM4CONST_TRANSPOSED);
			mat2[1] = mat[8];
			mat2[2] = mat[4];
			mat2[4] = mat[2];
			mat2[5] = mat[10];
			mat2[6] = mat[6];
			mat2[8] = mat[1];
			mat2[9] = mat[9];
			mat2[10] = mat[5];
			mat2[12] = mat[3];
			mat2[13] = mat[11];
			mat2[14] = mat[7];
			return mat2;
		}
		else
			return core::matrix4(mat, core::matrix4::EM4CONST_TRANSPOSED);
	}

	ITexture* loadDaeTexture(const std::wstring& path, std::vector<std::string>& textureFolder)
	{
		char realFilePath[1024];
		CStringImp::convertUnicodeToUTF8(path.c_str(), realFilePath);
		return CTextureManager::getInstance()->getTexture(realFilePath, textureFolder);
	}

	std::wstring getImageWithId(const std::wstring& id, const ArrayImages& listImages)
	{
		int n = (int)listImages.size();
		for (int i = 0; i < n; i++)
		{
			if (listImages[i].id == id)
				return listImages[i].fileName;
		}
		return id;
	}

	video::ITexture* getTextureFromImage(std::wstring& id, const ArrayImages& listImages, std::vector<std::string>& textureFolder)
	{
		std::wstring textureName = getImageWithId(id, listImages);
		return loadDaeTexture(textureName, textureFolder);
	}

	video::ITexture* getTextureFromImage(std::wstring& uri, ArrayEffectParams& listEffectParam, ArrayImages& listImages, std::vector<std::string>& textureFolder)
	{
		if (listImages.size() == 0)
			return NULL;

		int n = (int)listEffectParam.size();
		for (int i = 0; i < n; i++)
		{
			if (listEffectParam[i].Name == uri)
			{
				if (listEffectParam[i].InitFromTexture.size() > 0)
				{
					// load texture
					std::wstring textureName = getImageWithId(listEffectParam[i].InitFromTexture, listImages);
					return loadDaeTexture(textureName, textureFolder);
				}
				else if (listEffectParam[i].Source.size() > 0)
				{
					// try search parent ref
					return getTextureFromImage(listEffectParam[i].Source, listEffectParam, listImages, textureFolder);
				}

				return NULL;
			}
		}
		return NULL;
	}

	int getBufferWithUri(std::wstring& uri, SMeshParam* mesh)
	{
		int n = (int)mesh->Buffers.size();
		for (int i = 0; i < n; i++)
		{
			if (mesh->Buffers[i].Name == uri)
			{
				return i;
			}
		}
		return -1;
	}

	int getVerticesWithUri(std::wstring& uri, SMeshParam* mesh)
	{
		int n = (int)mesh->Vertices.size();
		for (int i = 0; i < n; i++)
		{
			if (mesh->Vertices[i].Name == uri)
			{
				return i;
			}
		}

		return -1;
	}

	int getEffectWithUri(std::wstring& uri, ArrayEffects& listEffectParam)
	{
		int n = (int)listEffectParam.size();
		for (int i = 0; i < n; i++)
		{
			if (listEffectParam[i].Id == uri)
			{
				return i;
			}
		}

		return -1;
	}

	int getEffectWithUri(std::wstring& uri, ArrayEffects& listEffectParam, ArrayMaterials& listMaterial)
	{
		int id = getEffectWithUri(uri, listEffectParam);
		if (id >= 0)
			return id;

		// try search in material list
		int n = (int)listMaterial.size();
		for (int i = 0; i < n; i++)
		{
			SColladaMaterial& material = listMaterial[i];

			if (material.ID == uri)
			{
				if (material.InstanceEffect.empty())
				{
					// rename to effect style
					listEffectParam.push_back(material.Effect);
					return (int)listEffectParam.size() - 1;
				}
				else
				{
					id = getEffectWithUri(material.InstanceEffect, listEffectParam);
					return id;
				}
			}
		}

		return -1;
	}

	int getMeshWithUri(std::wstring& uri, ArrayMeshParams& listMeshParam)
	{
		int n = (int)listMeshParam.size();
		for (int i = 0; i < n; i++)
		{
			if (listMeshParam[i].Name == uri)
			{
				return i;
			}
		}

		return -1;
	}

	int getMeshWithControllerName(std::wstring& controllerName, ArrayMeshParams& listMeshParam)
	{
		int n = (int)listMeshParam.size();
		for (int i = 0; i < n; i++)
		{
			if (listMeshParam[i].ControllerName == controllerName)
			{
				return i;
			}
		}

		return -1;
	}

}
