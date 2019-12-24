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

#ifndef _COLLADALOADER_FUNC_
#define _COLLADALOADER_FUNC_

#include "pch.h"
#include "CColladaLoader.h"

namespace Skylicht
{
	void uriToId(std::wstring& str);
	std::wstring readId(io::IXMLReader *xmlRead);
	std::wstring readName(io::IXMLReader *xmlRead);
	void findNextNoneWhiteSpace(const c8** start);
	inline f32 readFloat(const c8** p);

	void readIntsInsideElement(io::IXMLReader* reader, s32* ints, u32 count);
	void readIntsInsideElement(io::IXMLReader* reader, std::vector<s32>& arrayInt);
	void readFloatsInsideElement(io::IXMLReader* reader, f32* floats, u32 count);
	void readStringInsideElement(io::IXMLReader* reader, std::wstring& string);

	int	calcNumTriangleFromPolygon(const s32 *vCount, int numPolygon);
	int	convertPolygonToTriangle(const s32 *sourcePolygon, int offset, s32 *targetTriangle, int &targetID, int numVertex, int numElementPerVertex);

	SColorf readColorNode(io::IXMLReader* reader);
	f32 readFloatNode(io::IXMLReader* reader);
	core::matrix4 readTranslateNode(io::IXMLReader* reader, bool flip);
	core::matrix4 readRotateNode(io::IXMLReader* reader, bool flip);
	core::matrix4 readScaleNode(io::IXMLReader* reader, bool flip);
	core::matrix4 readMatrixNode(io::IXMLReader* reader, bool flip);

	ITexture* getTextureFromImage(std::wstring& uri, ArrayEffectParams& listEffectParam, ArrayImages& listImages, std::vector<std::string>& textureFolder);
	ITexture* getTextureFromImage(std::wstring& id, const ArrayImages& listImages, std::vector<std::string>& textureFolder);
	std::wstring getImageWithId(const std::wstring& id, const ArrayImages& listImages);
	ITexture* loadDaeTexture(const std::wstring& path, std::vector<std::string>& textureFolder);

	int getBufferWithUri(std::wstring& uri, SMeshParam* mesh);
	int	getVerticesWithUri(std::wstring& uri, SMeshParam* mesh);
	int	getEffectWithUri(std::wstring& uri, ArrayEffects& listEffectParam);
	int	getEffectWithUri(std::wstring& uri, ArrayEffects& listEffectParam, ArrayMaterials& listMaterial);
	int	getMeshWithUri(std::wstring& uri, ArrayMeshParams& listMeshParam);
	int	getMeshWithControllerName(std::wstring& controllerName, ArrayMeshParams& listMeshParam);
}

#endif