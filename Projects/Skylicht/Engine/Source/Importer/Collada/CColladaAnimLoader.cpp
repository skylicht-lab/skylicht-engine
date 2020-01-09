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
#include "Utils/CStringImp.h"

#include "CColladaLoader.h"
#include "CColladaLoaderFunc.h"

#include "CColladaAnimLoader.h"

#include "Animation/CAnimation.h"
#include "Animation/CAnimationClip.h"

namespace Skylicht
{
	CColladaAnimLoader::CColladaAnimLoader()
	{
	}

	CColladaAnimLoader::~CColladaAnimLoader()
	{

	}

	bool CColladaAnimLoader::loadAnimation(const char *resource, CAnimationClip* output)
	{
		IrrlichtDevice *device = getIrrlichtDevice();
		io::IFileSystem *fs = device->getFileSystem();

		io::IXMLReader *xmlRead = fs->createXMLReader(resource);

		if (xmlRead == NULL)
		{
			return false;
		}

		m_zUp = false;
		m_flipOx = false;

		bool readLUpAxis = false;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				core::stringw nodeName = xmlRead->getNodeName();

				if (core::stringw(L"animation") == nodeName)
				{
					if (xmlRead->getAttributeValue(L"id") != NULL)
					{
						parseAnimationNode(xmlRead);
					}
					else
					{
						parseAnimationSourceNode(xmlRead);
					}
				}
				else if (core::stringw(L"visual_scene") == nodeName)
				{
					parseSceneNode(xmlRead);
				}
				else if (nodeName == L"up_axis")
				{
					readLUpAxis = true;
				}
			}
			break;
			case io::EXN_ELEMENT_END:
			{
			}
			break;
			case io::EXN_TEXT:
			{
				if (readLUpAxis == true)
				{
					std::wstring text = xmlRead->getNodeData();
					if (text == L"Z_UP")
					{
						m_zUp = true;
						m_flipOx = false;
					}
					else
					{
						m_zUp = false;
						m_flipOx = true;
					}
				}
				readLUpAxis = false;
			}
			break;
			}
		}

		// construct anim data
		constructAnimation(resource, output);

		// clear memory data
		clearData();

		// close file
		xmlRead->drop();

		return true;
	}

	void CColladaAnimLoader::constructAnimation(const char *fileName, CAnimationClip* clip)
	{
		char name[512];
		CStringImp::getFileNameNoExt(name, fileName);
		clip->AnimName = name;
		clip->Time = 0;

		float frames = 0.0f;

		for (int j = 0; j < (int)m_listNode.size(); j++)
		{
			SNodeParam* pNode = m_listNode[j];

			std::stack<SNodeParam*>	stackNode;
			stackNode.push(pNode);
			while (stackNode.size())
			{
				pNode = stackNode.top();
				stackNode.pop();

				// found node
				CStringImp::convertUnicodeToUTF8(pNode->Name.c_str(), name);

				SEntityAnim *nodeAnim = m_nodeAnim[name];
				if (nodeAnim == NULL)
				{
					nodeAnim = new SEntityAnim();
					nodeAnim->Name = name;

					// default value
					core::matrix4 mat;
					nodeAnim->Data.DefaultRot = core::quaternion(pNode->Transform);
					nodeAnim->Data.DefaultPos = pNode->Transform.getTranslation();
					nodeAnim->Data.DefaultScale = pNode->Transform.getScale();

					// add node anim
					m_nodeAnim[name] = nodeAnim;
				}
				else
				{
					nodeAnim->Data.DefaultRot = core::quaternion(pNode->Transform);
					nodeAnim->Data.DefaultPos = pNode->Transform.getTranslation();
					nodeAnim->Data.DefaultScale = pNode->Transform.getScale();
				}


				for (int i = 0; i < (int)pNode->Childs.size(); i++)
					stackNode.push(pNode->Childs[i]);
			}
		}

		std::map<std::string, SEntityAnim*>::iterator iNodeAnim = m_nodeAnim.begin(), iNodeEnd = m_nodeAnim.end();
		while (iNodeAnim != iNodeEnd)
		{
			SEntityAnim* nodeAnim = iNodeAnim->second;
			SEntityAnim* newNodeAnim = new SEntityAnim();

			*newNodeAnim = *nodeAnim;

			// get default matrix
			SNodeParam *n = getNode(newNodeAnim->Name);
			if (n)
			{
				newNodeAnim->Data.DefaultRot = core::quaternion(n->Transform);
				newNodeAnim->Data.DefaultPos = n->Transform.getTranslation();
				newNodeAnim->Data.DefaultScale = n->Transform.getScale();
			}

			if (nodeAnim->Data.PositionKeys.size())
			{
				if (frames < nodeAnim->Data.PositionKeys.getLast().frame)
					frames = nodeAnim->Data.PositionKeys.getLast().frame;
			}
			if (nodeAnim->Data.RotationKeys.size())
			{
				if (frames < nodeAnim->Data.RotationKeys.getLast().frame)
					frames = nodeAnim->Data.RotationKeys.getLast().frame;
			}
			if (nodeAnim->Data.ScaleKeys.size())
			{
				if (frames < nodeAnim->Data.ScaleKeys.getLast().frame)
					frames = nodeAnim->Data.ScaleKeys.getLast().frame;
			}

			clip->addAnim(newNodeAnim);
			iNodeAnim++;
		}

		// n frames
		clip->Duration = frames;
	}

	void CColladaAnimLoader::clearData()
	{
		// clear node param data
		for (int j = 0; j < (int)m_listNode.size(); j++)
		{
			SNodeParam* pNode = m_listNode[j];

			std::stack<SNodeParam*>	stackNode;
			stackNode.push(pNode);
			while (stackNode.size())
			{
				pNode = stackNode.top();
				stackNode.pop();

				for (int i = 0; i < (int)pNode->Childs.size(); i++)
					stackNode.push(pNode->Childs[i]);

				delete pNode;
			}
		}
		m_listNode.clear();

		// clear node anim data
		std::map<std::string, SEntityAnim*>::iterator iNodeAnim = m_nodeAnim.begin(), iNodeEnd = m_nodeAnim.end();
		while (iNodeAnim != iNodeEnd)
		{
			delete iNodeAnim->second;
			iNodeAnim++;
		}
		m_nodeAnim.clear();
	}

	void CColladaAnimLoader::parseAnimationNode(io::IXMLReader *xmlRead)
	{
		std::wstring idNodeName = xmlRead->getAttributeValue(L"id");

		std::wstring jointName;
		bool isRotation = false;
		bool isTranslate = false;
		bool isScale = false;
		bool isMatrix = false;

		int pos = (int)idNodeName.find(L"-rotation");
		if (pos > 0)
		{
			isRotation = true;
			jointName = idNodeName.substr(0, pos);
		}
		else
		{
			pos = (int)idNodeName.find(L"-translation");
			if (pos > 0)
			{
				isTranslate = true;
				jointName = idNodeName.substr(0, pos);
			}
			else
			{
				pos = (int)idNodeName.find(L"-scale");
				if (pos > 0)
				{
					isScale = true;
					jointName = idNodeName.substr(0, pos);
				}
				else
				{
					pos = (int)idNodeName.find(L"-anim");
					if (pos > 0)
					{
						isMatrix = true;
						jointName = idNodeName.substr(0, pos);
					}
				}
			}
		}

		if (isRotation == false && isTranslate == false && isScale == false && isMatrix == false)
		{
			return;
		}


		char stringBuffer[1024];
		CStringImp::convertUnicodeToUTF8(jointName.c_str(), stringBuffer);

		// create anim node
		SEntityAnim *nodeAnim = m_nodeAnim[stringBuffer];
		if (nodeAnim == NULL)
		{
			nodeAnim = new SEntityAnim();
			nodeAnim->Name = stringBuffer;

			// default value
			core::matrix4 mat;
			nodeAnim->Data.DefaultRot = core::quaternion(mat);
			nodeAnim->Data.DefaultPos = mat.getTranslation();
			nodeAnim->Data.DefaultScale = mat.getScale();

			// add node anim
			m_nodeAnim[stringBuffer] = nodeAnim;
		}

		int readState = 0;

		float *arrayTime = NULL;
		float *arrayFloat = NULL;

		bool needReadDefaultRotValue = false;
		bool needReadDefaultPosValue = false;
		bool applyDefaultPos = false;

		std::wstring arrayID;
		int count = 0;

		int nAnimationTags = 1;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				core::stringw nodeName = xmlRead->getNodeName();

				if (core::stringw(L"animation") == nodeName)
				{
					nAnimationTags++;
				}
				else if (core::stringw(L"float_array") == nodeName)
				{
					arrayID = xmlRead->getAttributeValue(L"id");
					count = xmlRead->getAttributeValueAsInt(L"count");

					if ((int)arrayID.find(L"-input-array") > 0)
					{
						readState = 1;
						arrayTime = new float[count];
					}
					else if (
						(int)arrayID.find(L"-output-array") > 0 ||
						(int)arrayID.find(L"-output-transform-array") > 0
						)
					{
						readState = 2;
						arrayFloat = new float[count];
					}
				}
				else if (core::stringw(L"accessor") == nodeName && arrayFloat != NULL)
				{
					int stride = xmlRead->getAttributeValueAsInt(L"stride");
					int nFrame = count / stride;

					float fvector[4];

					for (int i = 0; i < nFrame; i++)
					{
						if (isRotation)
						{
							if (stride == 4)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 4];
									fvector[1] = arrayFloat[i * 4 + 2];
									fvector[2] = arrayFloat[i * 4 + 1];
									fvector[3] = -arrayFloat[i * 4 + 3];
								}
								else
								{
									fvector[0] = arrayFloat[i * 4];
									fvector[1] = arrayFloat[i * 4 + 1];
									fvector[2] = arrayFloat[i * 4 + 2];
									fvector[3] = arrayFloat[i * 4 + 3];
								}

								CFrameData::SRotationKey key;
								key.frame = arrayTime[i];
								key.rotation.fromAngleAxis(
									fvector[3] * core::DEGTORAD,
									core::vector3df(fvector[0], fvector[1], fvector[2])
								);
								nodeAnim->Data.RotationKeys.push_back(key);
							}
							else if (stride == 1)
							{
								needReadDefaultRotValue = true;
								break;
							}
							else
							{
								printf("Warning: May be not support stride: %d on rotate!\n", stride);
							}
						}
						else if (isTranslate)
						{
							if (stride == 3)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 2];
									fvector[2] = arrayFloat[i * 3 + 1];
								}
								else
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 1];
									fvector[2] = arrayFloat[i * 3 + 2];
								}

								CFrameData::SPositionKey key;
								key.frame = arrayTime[i];
								key.position = core::vector3df(fvector[0], fvector[1], fvector[2]);
								nodeAnim->Data.PositionKeys.push_back(key);
							}
							else if (stride == 1)
							{
								needReadDefaultPosValue = true;
								break;
							}
							else
								printf("Warning: May be not support stride: %d on translate!\n", stride);
						}
						else if (isScale)
						{
							if (stride == 3)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 2];
									fvector[2] = arrayFloat[i * 3 + 1];
								}
								else
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 1];
									fvector[2] = arrayFloat[i * 3 + 2];
								}

								CFrameData::SScaleKey key;
								key.frame = arrayTime[i];
								key.scale = core::vector3df(fvector[0], fvector[1], fvector[2]);
								nodeAnim->Data.ScaleKeys.push_back(key);
							}
							else
								printf("Warning: May be not support stride: %d on scale!\n", stride);
						}
						else if (isMatrix && stride == 16)
						{
							core::matrix4 mat;
							mat.setM(arrayFloat + i * 16);

							if (m_zUp)
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
								mat = mat2;
							}
							else
								mat = core::matrix4(mat, core::matrix4::EM4CONST_TRANSPOSED);

							CFrameData::SRotationKey key;
							key.frame = arrayTime[i];
							key.rotation = core::quaternion(mat);
							nodeAnim->Data.RotationKeys.push_back(key);


							CFrameData::SPositionKey keyPos;
							keyPos.frame = arrayTime[i];
							keyPos.position = mat.getTranslation();
							nodeAnim->Data.PositionKeys.push_back(keyPos);


							CFrameData::SScaleKey keyScale;
							keyScale.frame = arrayTime[i];
							keyScale.scale = mat.getScale();
							nodeAnim->Data.ScaleKeys.push_back(keyScale);
						}
						else
						{
							printf("Warning: May be not support some animation!\n");
						}

					}

					if (needReadDefaultRotValue == false && needReadDefaultPosValue == false)
					{
						delete arrayTime;
						arrayTime = NULL;

						delete arrayFloat;
						arrayFloat = NULL;
					}

				}
				else if (core::stringw(L"default_values") == nodeName && isRotation)
				{
					float fvector[4] = { 0 };

					parseDefaultValueRotate(xmlRead, &fvector[0], &fvector[1], &fvector[2], &fvector[3]);

					if (m_zUp == true)
					{
						float t = fvector[1];
						fvector[1] = fvector[2];
						fvector[2] = t;
					}

					if (needReadDefaultRotValue == true)
					{
						for (int i = 0; i < count; i++)
						{
							CFrameData::SRotationKey key;
							key.frame = arrayTime[i];

							float f = arrayFloat[i];
							if (m_zUp == true)
								f = -f;

							key.rotation.fromAngleAxis(
								f * core::DEGTORAD,
								core::vector3df(fvector[0], fvector[1], fvector[2])
							);
							nodeAnim->Data.RotationKeys.push_back(key);
						}

						delete arrayTime;
						arrayTime = NULL;

						delete arrayFloat;
						arrayFloat = NULL;

						needReadDefaultRotValue = false;
					}
				}
				else if (core::stringw(L"param") == nodeName && needReadDefaultPosValue)
				{
					core::stringw axis = xmlRead->getAttributeValue(L"name");

					int state = 0;
					if (axis == L"Y")
						state = 1;
					else if (axis == L"Z")
						state = 2;

					for (int i = 0; i < count; i++)
					{
						CFrameData::SPositionKey key;
						key.frame = arrayTime[i];

						if (state == 0)
							key.position = core::vector3df(arrayFloat[i], 0, 0);
						else if (state == 1)
							key.position = core::vector3df(0, arrayFloat[i], 0);
						else
							key.position = core::vector3df(0, 0, arrayFloat[i]);

						if (m_zUp == true)
						{
							float t = key.position.Y;
							key.position.Y = key.position.Z;
							key.position.Z = t;
						}

						nodeAnim->Data.PositionKeys.push_back(key);
					}

					delete arrayTime;
					arrayTime = NULL;

					delete arrayFloat;
					arrayFloat = NULL;

					needReadDefaultPosValue = false;
					applyDefaultPos = true;
				}
				else if (core::stringw(L"default_values") == nodeName && isTranslate)
				{
					float	fvector[4] = { 0 };
					parseDefaultValuePosition(xmlRead, &fvector[0], &fvector[1], &fvector[2]);

					if (m_zUp == true)
					{
						float t = fvector[1];
						fvector[1] = fvector[2];
						fvector[2] = t;
					}

					if (applyDefaultPos)
					{
						// apply position
						for (int i = 0, n = nodeAnim->Data.PositionKeys.size(); i < n; i++)
						{
							if (nodeAnim->Data.PositionKeys[i].position.X == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.X += fvector[0];

							if (nodeAnim->Data.PositionKeys[i].position.Y == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.Y += fvector[1];

							if (nodeAnim->Data.PositionKeys[i].position.Z == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.Z += fvector[2];
						}
						applyDefaultPos = false;
					}

				}
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"animation") == nodeName)
				{
					nAnimationTags--;
					if (nAnimationTags == 0)
						return;
				}
			}
			break;
			case io::EXN_TEXT:
			{
				if (readState == 1 || readState == 2)
				{
					const wchar_t *data = xmlRead->getNodeData();

					wchar_t* p = (wchar_t*)data;
					wchar_t* begin = (wchar_t*)data;

					float value = 0;
					int numArrayTime = 0;
					int numArray = 0;


					int len = CStringImp::length<const wchar_t>(data);
					while (*p && len > 0)
					{
						while (*p && !(*p == L' ' || *p == L'\n' || *p == L'\r' || *p == L'\t'))
						{
							++p;
							len--;
						}

						*p = 0;

						if (*begin)
						{
							swscanf(begin, L"%f", &value);

							if (readState == 1)
							{
								// read time
								arrayTime[numArrayTime++] = value;
							}
							else if (readState == 2)
							{
								// add to list
								arrayFloat[numArray++] = value;
							}
						}

						p++;
						len--;
						begin = p;
					}
				}
				readState = 0;
			}
			break;
			}
		}
	}

	void CColladaAnimLoader::parseAnimationSourceNode(io::IXMLReader *xmlRead)
	{
		std::wstring jointName;

		bool isRotation = false;
		bool isTranslate = false;
		bool isScale = false;
		bool isMatrix = false;

		int isTranslateXYZ = 0;
		int isRotateXYZ = 0;

		int readState = 0;

		float *arrayTime = NULL;
		float *arrayFloat = NULL;

		bool needReadDefaultRotValue = false;
		bool needReadDefaultPosValue = false;
		bool applyDefaultPos = false;

		std::wstring arrayID;
		int count = 0;

		int nAnimationTags = 1;

		SEntityAnim *nodeAnim = NULL;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				core::stringw nodeName = xmlRead->getNodeName();

				if (core::stringw(L"animation") == nodeName)
				{
					nAnimationTags++;
				}
				else if (core::stringw(L"source") == nodeName)
				{
					if (xmlRead->getAttributeValue(L"id") != NULL)
					{
						std::wstring idNodeName = xmlRead->getAttributeValue(L"id");

						// ROTATION KEY
						int pos = (int)idNodeName.find(L"_rotation-input");
						if (pos > 0)
						{
							isRotation = false;
							isTranslate = false;
							isScale = false;
							isMatrix = false;
							isTranslateXYZ = 0;
							isRotateXYZ = 0;
							// ------------

							isRotation = true;
							jointName = idNodeName.substr(0, pos);
						}

						// TRANSLATE KEY
						if (pos < 0)
						{
							pos = (int)idNodeName.find(L"_translation-input");
							if (pos > 0)
							{
								isRotation = false;
								isTranslate = false;
								isScale = false;
								isMatrix = false;
								isTranslateXYZ = 0;
								isRotateXYZ = 0;
								// ------------
								isTranslate = true;
								jointName = idNodeName.substr(0, pos);
							}
						}

						// SCALE KEY
						if (pos < 0)
						{
							pos = (int)idNodeName.find(L"_scale-input");
							if (pos > 0)
							{
								isRotation = false;
								isTranslate = false;
								isScale = false;
								isMatrix = false;
								isTranslateXYZ = 0;
								isRotateXYZ = 0;
								// ------------
								isScale = true;
								jointName = idNodeName.substr(0, pos);
							}
						}

						// MATRIX KEY
						if (pos < 0)
						{
							const wchar_t* matrixInput[] =
							{
								L"_anim-input",
								L"_matrix-input",
								NULL
							};
							int i = 0;

							while (matrixInput[i] != NULL)
							{
								pos = (int)idNodeName.find(matrixInput[i]);
								if (pos > 0)
								{
									isRotation = false;
									isTranslate = false;
									isScale = false;
									isMatrix = false;
									isTranslateXYZ = 0;
									isRotateXYZ = 0;
									// ------------
									isMatrix = true;
									jointName = idNodeName.substr(0, pos);
									break;
								}
								i++;
							}
						}

						// TRANSLATE ELEMENT KEY
						if (pos < 0)
						{
							const wchar_t* translateInput[] =
							{
								L"_translation.X-input",
								L"_translation.Y-input",
								L"_translation.Z-input",
								NULL,
							};
							int i = 0;
							while (translateInput[i] != NULL)
							{
								pos = (int)idNodeName.find(translateInput[i]);
								if (pos > 0)
								{
									isRotation = false;
									isTranslate = false;
									isScale = false;
									isMatrix = false;
									isTranslateXYZ = 0;
									isRotateXYZ = 0;

									isTranslateXYZ = i + 1;
									jointName = idNodeName.substr(0, pos);
									break;
								}
								i++;
							}
						}

						// ROTATE ELEMENT KEY
						if (pos < 0)
						{
							const wchar_t* rotateInput[] =
							{
								L"_rotationX.ANGLE-input",
								L"_rotationY.ANGLE-input",
								L"_rotationZ.ANGLE-input",
								NULL,
							};
							int i = 0;
							while (rotateInput[i] != NULL)
							{
								pos = (int)idNodeName.find(rotateInput[i]);
								if (pos > 0)
								{
									isRotation = false;
									isTranslate = false;
									isScale = false;
									isMatrix = false;
									isTranslateXYZ = 0;
									isRotateXYZ = 0;
									// ------------
									isRotateXYZ = i + 1;
									jointName = idNodeName.substr(0, pos);
									break;
								}
								i++;
							}
						}

						if (isRotation == true ||
							isTranslate == true ||
							isScale == true ||
							isMatrix == true ||
							isRotateXYZ > 0 ||
							isTranslateXYZ > 0)
						{
							char stringBuffer[1024];
							CStringImp::convertUnicodeToUTF8(jointName.c_str(), stringBuffer);

							// create anim node
							nodeAnim = m_nodeAnim[stringBuffer];
							if (nodeAnim == NULL)
							{
								nodeAnim = new SEntityAnim();
								nodeAnim->Name = stringBuffer;

								// default value
								core::matrix4 mat;
								nodeAnim->Data.DefaultRot = core::quaternion(mat);
								nodeAnim->Data.DefaultPos = mat.getTranslation();
								nodeAnim->Data.DefaultScale = mat.getScale();

								// add node anim
								m_nodeAnim[stringBuffer] = nodeAnim;
							}

						}
					}
				}
				else if (core::stringw(L"float_array") == nodeName)
				{
					arrayID = xmlRead->getAttributeValue(L"id");
					count = xmlRead->getAttributeValueAsInt(L"count");

					if ((int)arrayID.find(L"-input-array") > 0)
					{
						readState = 1;
						arrayTime = new float[count];
					}
					else if ((int)arrayID.find(L"-output-array") > 0 || (int)arrayID.find(L"-output-transform-array") > 0)
					{
						readState = 2;
						arrayFloat = new float[count];
					}
				}
				else if (core::stringw(L"accessor") == nodeName && arrayFloat != NULL)
				{
					int stride = xmlRead->getAttributeValueAsInt(L"stride");
					int nFrame = count / stride;

					float fvector[4] = { 0.0f };

					for (int i = 0; i < nFrame; i++)
					{
						if (isRotation)
						{
							if (stride == 4)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 4];
									fvector[1] = arrayFloat[i * 4 + 2];
									fvector[2] = arrayFloat[i * 4 + 1];
									fvector[3] = -arrayFloat[i * 4 + 3];
								}
								else
								{
									fvector[0] = arrayFloat[i * 4];
									fvector[1] = arrayFloat[i * 4 + 1];
									fvector[2] = arrayFloat[i * 4 + 2];
									fvector[3] = arrayFloat[i * 4 + 3];
								}

								CFrameData::SRotationKey key;
								key.frame = arrayTime[i];
								key.rotation.fromAngleAxis(
									fvector[3] * core::DEGTORAD,
									core::vector3df(fvector[0], fvector[1], fvector[2])
								);

								if (nodeAnim)
									nodeAnim->Data.RotationKeys.push_back(key);
							}
							else if (stride == 1)
							{
								needReadDefaultRotValue = true;
								break;
							}
							else
							{
								printf("Warning: May be not support stride: %d on rotate!\n", stride);
							}
						}
						else if (isTranslate)
						{
							if (stride == 3)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 2];
									fvector[2] = arrayFloat[i * 3 + 1];
								}
								else
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 1];
									fvector[2] = arrayFloat[i * 3 + 2];
								}

								CFrameData::SPositionKey key;
								key.frame = arrayTime[i];
								key.position = core::vector3df(fvector[0], fvector[1], fvector[2]);

								if (nodeAnim)
									nodeAnim->Data.PositionKeys.push_back(key);
							}
							else if (stride == 1)
							{
								needReadDefaultPosValue = true;
								break;
							}
							else
								printf("Warning: May be not support stride: %d on translate!\n", stride);
						}
						else if (isScale)
						{
							if (stride == 3)
							{
								if (m_zUp == true)
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 2];
									fvector[2] = arrayFloat[i * 3 + 1];
								}
								else
								{
									fvector[0] = arrayFloat[i * 3];
									fvector[1] = arrayFloat[i * 3 + 1];
									fvector[2] = arrayFloat[i * 3 + 2];
								}

								CFrameData::SScaleKey key;
								key.frame = arrayTime[i];
								key.scale = core::vector3df(fvector[0], fvector[1], fvector[2]);

								if (nodeAnim)
									nodeAnim->Data.ScaleKeys.push_back(key);
							}
							else
								printf("Warning: May be not support stride: %d on scale!\n", stride);
						}
						else if (isMatrix && stride == 16)
						{
							core::matrix4 mat;
							mat.setM(arrayFloat + i * 16);

							if (m_zUp)
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
								mat = mat2;
							}
							else
								mat = core::matrix4(mat, core::matrix4::EM4CONST_TRANSPOSED);

							CFrameData::SRotationKey key;
							key.frame = arrayTime[i];
							key.rotation = core::quaternion(mat);

							if (nodeAnim)
								nodeAnim->Data.RotationKeys.push_back(key);


							CFrameData::SPositionKey keyPos;
							keyPos.frame = arrayTime[i];
							keyPos.position = mat.getTranslation();

							if (nodeAnim)
								nodeAnim->Data.PositionKeys.push_back(keyPos);

							CFrameData::SScaleKey keyScale;
							keyScale.frame = arrayTime[i];
							keyScale.scale = mat.getScale();

							if (nodeAnim)
								nodeAnim->Data.ScaleKeys.push_back(keyScale);
						}
						else if (isTranslateXYZ > 0 && stride == 1)
						{
							if (m_zUp == true)
							{
								if (isTranslateXYZ == 1)
									fvector[0] = arrayFloat[i];
								else if (isTranslateXYZ == 2)
									fvector[2] = arrayFloat[i];
								else
									fvector[1] = arrayFloat[i];
							}
							else
							{
								if (isTranslateXYZ == 1)
									fvector[0] = arrayFloat[i];
								else if (isTranslateXYZ == 2)
									fvector[1] = arrayFloat[i];
								else
									fvector[2] = arrayFloat[i];
							}

							if (nodeAnim)
							{
								if ((int)nodeAnim->Data.PositionKeys.size() <= i)
								{
									CFrameData::SPositionKey key;
									key.frame = arrayTime[i];
									key.position = core::vector3df(fvector[0], fvector[1], fvector[2]);
									nodeAnim->Data.PositionKeys.push_back(key);
								}
								else
								{
									CFrameData::SPositionKey &key = nodeAnim->Data.PositionKeys[i];
									key.position += core::vector3df(fvector[0], fvector[1], fvector[2]);
								}
							}
						}
						else if (isRotateXYZ > 0 && stride == 1)
						{
							if (m_zUp == true)
							{
								if (isRotateXYZ == 1)
								{
									fvector[0] = 1.0f;
									fvector[1] = 0.0f;
									fvector[2] = 0.0f;
								}
								else if (isRotateXYZ == 2)
								{
									fvector[0] = 0.0f;
									fvector[1] = 0.0f;
									fvector[2] = 1.0f;
								}
								else
								{
									fvector[0] = 0.0f;
									fvector[1] = 1.0f;
									fvector[2] = 0.0f;
								}
								fvector[3] = -arrayFloat[i];
							}
							else
							{
								if (isRotateXYZ == 1)
								{
									fvector[0] = 1.0f;
									fvector[1] = 0.0f;
									fvector[2] = 0.0f;
								}
								else if (isRotateXYZ == 2)
								{
									fvector[0] = 0.0f;
									fvector[1] = 1.0f;
									fvector[2] = 0.0f;
								}
								else
								{
									fvector[0] = 0.0f;
									fvector[1] = 0.0f;
									fvector[2] = 1.0f;
								}
								fvector[3] = arrayFloat[i];
							}

							if (nodeAnim)
							{
								if ((int)nodeAnim->Data.RotationKeys.size() <= i)
								{
									CFrameData::SRotationKey key;
									key.frame = arrayTime[i];
									key.rotation.fromAngleAxis(
										fvector[3] * core::DEGTORAD,
										core::vector3df(fvector[0], fvector[1], fvector[2])
									);
									nodeAnim->Data.RotationKeys.push_back(key);
								}
								else
								{
									core::quaternion rotation;
									rotation.fromAngleAxis(
										fvector[3] * core::DEGTORAD,
										core::vector3df(fvector[0], fvector[1], fvector[2])
									);
									nodeAnim->Data.RotationKeys[i].rotation *= rotation;
								}
							}
						}
						else
						{
							printf("Warning: May be not support some animation!\n");
						}
					}

					if (needReadDefaultRotValue == false && needReadDefaultPosValue == false)
					{
						delete arrayTime;
						arrayTime = NULL;

						delete arrayFloat;
						arrayFloat = NULL;
					}

				}
				else if (core::stringw(L"default_values") == nodeName && isRotation)
				{
					float	fvector[4] = { 0 };

					parseDefaultValueRotate(xmlRead, &fvector[0], &fvector[1], &fvector[2], &fvector[3]);

					if (m_zUp == true)
					{
						float t = fvector[1];
						fvector[1] = fvector[2];
						fvector[2] = t;
					}

					if (needReadDefaultRotValue == true)
					{
						for (int i = 0; i < count; i++)
						{
							CFrameData::SRotationKey key;
							key.frame = arrayTime[i];

							float f = arrayFloat[i];
							if (m_zUp == true)
								f = -f;

							/*
							if (fvector[0] == 0.0f &&
								fvector[1] == 0.0f &&
								fvector[2] == 0.0f)
							{
								key.rotation.makeIdentity();
							}
							else
							*/
							{
								key.rotation.fromAngleAxis(
									f * core::DEGTORAD,
									core::vector3df(fvector[0], fvector[1], fvector[2])
								);
							}

							if (nodeAnim)
								nodeAnim->Data.RotationKeys.push_back(key);
						}

						delete arrayTime;
						arrayTime = NULL;

						delete arrayFloat;
						arrayFloat = NULL;

						needReadDefaultRotValue = false;
					}
				}
				else if (core::stringw(L"param") == nodeName && needReadDefaultPosValue)
				{
					core::stringw axis = xmlRead->getAttributeValue(L"name");

					int state = 0;
					if (axis == L"Y")
						state = 1;
					else if (axis == L"Z")
						state = 2;

					for (int i = 0; i < count; i++)
					{
						CFrameData::SPositionKey key;
						key.frame = arrayTime[i];

						if (state == 0)
							key.position = core::vector3df(arrayFloat[i], 0, 0);
						else if (state == 1)
							key.position = core::vector3df(0, arrayFloat[i], 0);
						else
							key.position = core::vector3df(0, 0, arrayFloat[i]);

						if (m_zUp == true)
						{
							float t = key.position.Y;
							key.position.Y = key.position.Z;
							key.position.Z = t;
						}

						if (nodeAnim)
							nodeAnim->Data.PositionKeys.push_back(key);
					}

					delete arrayTime;
					arrayTime = NULL;

					delete arrayFloat;
					arrayFloat = NULL;

					needReadDefaultPosValue = false;
					applyDefaultPos = true;
				}
				else if (core::stringw(L"default_values") == nodeName && isTranslate)
				{
					float	fvector[4] = { 0 };
					parseDefaultValuePosition(xmlRead, &fvector[0], &fvector[1], &fvector[2]);

					if (m_zUp == true)
					{
						float t = fvector[1];
						fvector[1] = fvector[2];
						fvector[2] = t;
					}

					if (applyDefaultPos && nodeAnim)
					{
						// apply position
						for (int i = 0, n = nodeAnim->Data.PositionKeys.size(); i < n; i++)
						{
							if (nodeAnim->Data.PositionKeys[i].position.X == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.X += fvector[0];

							if (nodeAnim->Data.PositionKeys[i].position.Y == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.Y += fvector[1];

							if (nodeAnim->Data.PositionKeys[i].position.Z == 0.0f)
								nodeAnim->Data.PositionKeys[i].position.Z += fvector[2];
						}
						applyDefaultPos = false;
					}

				}
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"animation") == nodeName)
				{
					nAnimationTags--;
					if (nAnimationTags == 0)
						return;
				}
			}
			break;
			case io::EXN_TEXT:
			{
				if (readState == 1 || readState == 2)
				{
					const wchar_t *data = xmlRead->getNodeData();

					wchar_t* p = (wchar_t*)data;
					wchar_t* begin = (wchar_t*)data;

					float value = 0;
					int numArrayTime = 0;
					int numArray = 0;


					int len = CStringImp::length<const wchar_t>(data);
					while (*p && len > 0)
					{
						while (*p && !(*p == L' ' || *p == L'\n' || *p == L'\r' || *p == L'\t'))
						{
							++p;
							len--;
						}

						*p = 0;

						if (*begin)
						{
							swscanf(begin, L"%f", &value);

							if (readState == 1)
							{
								// read time
								arrayTime[numArrayTime++] = value;
							}
							else if (readState == 2)
							{
								// add to list
								arrayFloat[numArray++] = value;
							}
						}

						p++;
						len--;
						begin = p;
					}
				}
				readState = 0;
			}
			break;
			}
		}
	}

	void CColladaAnimLoader::parseDefaultValuePosition(io::IXMLReader *xmlRead, float *x, float *y, float *z)
	{
		int readState = 0;
		*x = 0.0f;
		*y = 0.0f;
		*z = 0.0f;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"param") == nodeName)
				{
					if (xmlRead->getAttributeValue(L"name") != NULL)
					{
						core::stringw attb = xmlRead->getAttributeValue(L"name");
						if (attb == L"X" || attb == L"x")
							readState = 1;
						if (attb == L"Y" || attb == L"y")
							readState = 2;
						if (attb == L"Z" || attb == L"z")
							readState = 3;
					}
				}
			}
			break;
			case io::EXN_TEXT:
			{
				const wchar_t *data = xmlRead->getNodeData();

				if (readState == 1)
					swscanf(data, L"%f", x);
				else if (readState == 2)
					swscanf(data, L"%f", y);
				else if (readState == 3)
					swscanf(data, L"%f", z);

				readState = 0;
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"default_values") == nodeName)
					return;
			}
			break;
			}
		};
	}

	void CColladaAnimLoader::parseDefaultValueRotate(io::IXMLReader *xmlRead, float *x, float *y, float *z, float *angle)
	{
		int readState = 0;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"param") == nodeName)
					readState++;
			}
			break;
			case io::EXN_TEXT:
			{
				const wchar_t *data = xmlRead->getNodeData();

				if (readState == 1)
					swscanf(data, L"%f", x);
				else if (readState == 2)
					swscanf(data, L"%f", y);
				else if (readState == 3)
					swscanf(data, L"%f", z);
				else if (readState == 4)
					swscanf(data, L"%f", angle);
			}
			break;
			case io::EXN_ELEMENT_END:
			{
				core::stringw nodeName = xmlRead->getNodeName();
				if (core::stringw(L"default_values") == nodeName)
					return;
			}
			break;
			}
		};
	}

	void CColladaAnimLoader::parseSceneNode(io::IXMLReader *xmlRead)
	{
		const std::wstring sceneSectionName(L"visual_scene");
		const std::wstring nodeSectionName(L"node");

		m_colladaRoot = NULL;

		while (xmlRead->read())
		{
			std::wstring nodeName = xmlRead->getNodeName();

			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				if (nodeName == nodeSectionName)
				{
					parseNode(xmlRead, NULL);
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (nodeName == sceneSectionName)
					break;
			}
		}
	}

	SNodeParam* CColladaAnimLoader::parseNode(io::IXMLReader *xmlRead, SNodeParam* parent)
	{
		const std::wstring nodeSectionName(L"node");
		const std::wstring translateSectionName(L"translate");
		const std::wstring rotateSectionName(L"rotate");
		const std::wstring scaleSectionName(L"scale");
		const std::wstring matrixNodeName(L"matrix");

		const std::wstring instanceGeometrySectionName(L"instance_geometry");
		const std::wstring instanceControllerSectionName(L"instance_controller");

		SNodeParam *pNode = new SNodeParam();

		pNode->Name = L"";
		if (xmlRead->getAttributeValue(L"id"))
			pNode->Name = xmlRead->getAttributeValue(L"id");

		if (xmlRead->getAttributeValue(L"sid"))
			pNode->SID = xmlRead->getAttributeValue(L"sid");

		pNode->Type = L"NODE";
		if (xmlRead->getAttributeValue(L"type"))
			pNode->Type = xmlRead->getAttributeValue(L"type");

		if (m_colladaRoot == NULL)
		{
			m_colladaRoot = new SNodeParam();
			m_colladaRoot->Name = L"BoneRoot";
			m_colladaRoot->Type = L"JOINT";
			m_colladaRoot->ChildLevel = 0;

			m_colladaRoot->Parent = NULL;

			if (m_flipOx == true)
			{
				// Inverse X
				m_colladaRoot->Transform.setScale(core::vector3df(-1.0f, 1.0f, 1.0f));
			}

			if (m_zUp)
			{
				// Z is front
				core::matrix4 rot;
				core::vector3df rotVec(0.0f, 180.0, 0.0f);
				rot.setRotationDegrees(rotVec);

				m_colladaRoot->Transform *= rot;
			}

			m_listNode.push_back(m_colladaRoot);
		}

		if (parent == NULL)
			parent = m_colladaRoot;

		pNode->Parent = parent;
		if (parent)
		{
			parent->Childs.push_back(pNode);
			pNode->ChildLevel = parent->ChildLevel + 1;
		}
		else
			pNode->ChildLevel = 0;


		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				if (xmlRead->getNodeName() == nodeSectionName)
				{
					parseNode(xmlRead, pNode);
				}
				else if (xmlRead->getNodeName() == translateSectionName)
				{
					// mul translate
					core::matrix4 m = readTranslateNode(xmlRead, m_zUp);
					pNode->Transform *= m;
				}
				else if (xmlRead->getNodeName() == rotateSectionName)
				{
					// mul rotate
					core::matrix4 m = readRotateNode(xmlRead, m_zUp);
					pNode->Transform *= m;
				}
				else if (xmlRead->getNodeName() == scaleSectionName)
				{
					// mul scale
					core::matrix4 m = readScaleNode(xmlRead, m_zUp);
					pNode->Transform *= m;
				}
				else if (xmlRead->getNodeName() == matrixNodeName)
				{
					// mul matrix
					core::matrix4 m = readMatrixNode(xmlRead, m_zUp);
					pNode->Transform *= m;
				}
				else if (xmlRead->getNodeName() == instanceGeometrySectionName)
				{
					// <instance_geometry url="#MESHNAME">
					pNode->Instance = xmlRead->getAttributeValue(L"url");
					uriToId(pNode->Instance);
				}
				else if (xmlRead->getNodeName() == instanceControllerSectionName)
				{
					// <instance_controller url="#MESHNAME">

					// move to root
					if (pNode->Parent)
					{
						std::vector<SNodeParam*>::iterator i = std::find(pNode->Parent->Childs.begin(), pNode->Parent->Childs.end(), pNode);
						if (i != pNode->Parent->Childs.end())
							pNode->Parent->Childs.erase(i);

						pNode->ChildLevel = 0;
						pNode->Parent = NULL;
					}

					// get skin mesh url
					pNode->Instance = xmlRead->getAttributeValue(L"url");
					uriToId(pNode->Instance);
				}

			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (xmlRead->getNodeName() == nodeSectionName)
					break;
			}
		}

		if (pNode->Parent == NULL)
			m_listNode.push_back(pNode);

		return pNode;
	}

	SNodeParam* CColladaAnimLoader::getNode(const std::string& nodeName)
	{
		wchar_t wName[512];
		CStringImp::convertUTF8ToUnicode(nodeName.c_str(), wName);

		for (int j = 0; j < (int)m_listNode.size(); j++)
		{
			SNodeParam* pNode = m_listNode[j];

			std::stack<SNodeParam*>	stackNode;
			stackNode.push(pNode);
			while (stackNode.size())
			{
				pNode = stackNode.top();
				stackNode.pop();

				// found node
				if (pNode->Name == wName)
					return pNode;
				else
				{
					for (int i = 0; i < (int)pNode->Childs.size(); i++)
						stackNode.push(pNode->Childs[i]);
				}
			}
		}

		return NULL;
	}
}