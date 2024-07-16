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
#include "CColladaLoader.h"
#include "CColladaLoaderFunc.h"

#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

#include "GameObject/CGameObject.h"
#include "Material/Shader/CShaderManager.h"

#include "Entity/CEntityPrefab.h"
#include "RenderMesh/CRenderMeshData.h"
#include "RenderMesh/CJointData.h"
#include "Culling/CCullingData.h"
#include "Transform/CWorldTransformData.h"

#include "Importer/Utils/CMeshUtils.h"

namespace Skylicht
{

	bool CColladaLoader::s_fixUV = false;

	CColladaLoader::CColladaLoader() :
		m_loadTexcoord2(false),
		m_createBatchMesh(false),
		m_createTangent(false),
		m_loadNormalMap(false),
		m_flipNormalMap(true),
		m_maxUVTile(16.0f),
		m_unit(""),
		m_unitScale(1.0f)
	{

	}

	CColladaLoader::~CColladaLoader()
	{

	}

	bool CColladaLoader::loadModel(const char* resource, CEntityPrefab* output, bool normalMap, bool flipNormalMap, bool texcoord2, bool batching)
	{
		setLoadNormalMap(normalMap, flipNormalMap);
		setLoadTexCoord2(texcoord2);
		setCreateBatchMesh(batching);

		return loadDae(resource, output);
	}

	bool CColladaLoader::loadDae(const char* fileName, CEntityPrefab* output)
	{
		m_meshFile = fileName;
		m_meshName = CPath::getFileName(fileName);

		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IXMLReader* xmlRead = fs->createXMLReader(fileName);
		if (xmlRead == NULL)
		{
			char log[64];
			sprintf(log, "Can not load: %s\n", fileName);
			os::Printer::log(log);
			return false;
		}

		const std::wstring controllerSectionName(L"controller");
		const std::wstring imageSectionName(L"image");
		const std::wstring materialSectionName(L"material");
		const std::wstring effectSectionName(L"effect");
		const std::wstring geometrySectionName(L"geometry");
		const std::wstring sceneSectionName(L"visual_scene");

		bool readLUpAxis = false;
		m_zUp = false;
		m_flipOx = false;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlRead->getNodeName();

				if (nodeName == imageSectionName)
				{
					parseImageNode(xmlRead);
				}
				else if (nodeName == materialSectionName)
				{
					parseMaterialNode(xmlRead);
				}
				else if (nodeName == effectSectionName)
				{
					parseEffectNode(xmlRead);
				}
				else if (nodeName == geometrySectionName)
				{
					parseGeometryNode(xmlRead);
				}
				else if (nodeName == controllerSectionName)
				{
					parseControllerNode(xmlRead);
				}
				else if (nodeName == sceneSectionName)
				{
					parseSceneNode(xmlRead);
				}
				else if (nodeName == L"up_axis")
				{
					readLUpAxis = true;
				}
				else if (nodeName == L"unit")
				{
					parseUnit(xmlRead);
				}

			}
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
			default:
				break;
			}
		}

		xmlRead->drop();

		loadEffectTexture();

		constructEntityPrefab(output);

		cleanData();

		return true;
	}

	void CColladaLoader::parseUnit(io::IXMLReader* xmlRead)
	{
		char unitName[64] = { 0 };
		char unitValue[64] = { 0 };

		const wchar_t* unitNameW = xmlRead->getAttributeValue(L"name");
		if (unitNameW != NULL)
		{
			CStringImp::copy(unitName, unitNameW);
			m_unit = unitName;
		}

		const wchar_t* meterW = xmlRead->getAttributeValue(L"meter");
		if (meterW != NULL)
		{
			CStringImp::copy(unitValue, meterW);
			m_unitScale = (float)atof(unitValue);
		}
	}

	void CColladaLoader::parseImageNode(io::IXMLReader* xmlRead, SColladaImage* image)
	{
		if (image == NULL)
		{
			m_listImages.push_back(SColladaImage());
			image = &m_listImages.back();
		}

		const std::wstring imageSectionName(L"image");

		image->id = xmlRead->getAttributeValue(L"id");

		if (xmlRead->getAttributeValue(L"name"))
			image->name = xmlRead->getAttributeValue(L"name");
		else
			image->name = image->id;

		bool readPath = false;

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring nodeName = xmlRead->getNodeName();
				if (nodeName == L"init_from")
					readPath = true;
			}
			else if (xmlRead->getNodeType() == io::EXN_TEXT)
			{
				if (readPath)
					image->fileName = xmlRead->getNodeData();
				readPath = false;
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				std::wstring nodeName = xmlRead->getNodeName();
				if (nodeName == imageSectionName)
					break;
			}
		}
	}

	void CColladaLoader::parseMaterialNode(io::IXMLReader* xmlRead, SColladaMaterial* material)
	{
		if (material == NULL)
		{
			m_listMaterial.push_back(SColladaMaterial());
			material = &m_listMaterial.back();

			material->ID = readId(xmlRead);

			material->Effect.Id = material->ID;
			material->Effect.Mat.Lighting = true;
			material->Effect.Mat.NormalizeNormals = true;

			material->Effect.Transparency = 1.f;
			material->Effect.HasAlpha = false;
			material->Effect.TransparentAddColor = false;

			material->Effect.HasBumpMapping = false;
			material->Effect.HasSpecularMapping = false;
			material->Effect.HasDiffuseMapping = false;
		}

		const std::wstring setParam(L"setparam");
		const std::wstring materialSectionName(L"material");
		const std::wstring initFrom(L"init_from");
		const std::wstring instanceEffect(L"instance_effect");

		const std::wstring diffuseSurface(L"Diffuse-surface");
		const std::wstring diffuseMapSurface(L"DiffuseMap-surface");
		const std::wstring normalMapSurface(L"NormalMap-surface");
		const std::wstring specularMapSurface(L"SpecularMapSampler-surface");

		bool readInitFrom = 0;

		std::wstring paramName = L"";

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				// read the first image init as texture
				std::wstring nodeName = xmlRead->getNodeName();
				if (nodeName == initFrom)
				{
					readInitFrom = 1;
				}
				else if (nodeName == setParam)
				{
					paramName = xmlRead->getAttributeValue(L"ref");
				}
				else if (nodeName == instanceEffect)
				{
					material->InstanceEffect = xmlRead->getAttributeValue(L"url");
					uriToId(material->InstanceEffect);
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_TEXT)
			{
				// read texture
				if (readInitFrom == 1)
				{
					std::wstring tname = xmlRead->getNodeData();

					if (paramName == diffuseSurface || paramName == diffuseMapSurface)
					{
						material->Effect.DiffuseRef = tname;
						material->Effect.HasDiffuseMapping = true;
					}
					else if (paramName == specularMapSurface)
					{
						material->Effect.SpecularRef = tname;
						material->Effect.HasSpecularMapping = true;
					}
					else if (paramName == normalMapSurface)
					{
						material->Effect.BumpRef = tname;
						material->Effect.HasBumpMapping = true;
					}
					else
					{
						char text[512];
						core::stringc name = paramName.c_str();
						sprintf(text, "Unknown map %s", name.c_str());
						os::Printer::log(text);
					}
				}

				readInitFrom = 0;
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				// end parse material
				std::wstring nodeName = xmlRead->getNodeName();
				if (nodeName == materialSectionName)
					break;
				else if (nodeName == setParam)
					paramName = L"";
			}
		}

		if (material)
			updateEffectMaterial(&material->Effect);
	}

	void CColladaLoader::parseEffectNode(io::IXMLReader* xmlRead, SEffect* effect)
	{
		if (effect == NULL)
		{
			m_listEffects.push_back(SEffect());
			effect = &m_listEffects.back();

			effect->Id = readId(xmlRead);

			effect->Mat.Lighting = true;
			effect->Mat.NormalizeNormals = true;

			effect->Transparency = 1.f;
			effect->HasAlpha = false;
			effect->TransparentAddColor = false;

			effect->HasBumpMapping = false;
			effect->HasSpecularMapping = false;
		}

		const std::wstring constantNode(L"constant");
		const std::wstring lambertNode(L"lambert");
		const std::wstring phongNode(L"phong");
		const std::wstring blinnNode(L"blinn");
		const std::wstring bumpNode(L"bump");
		const std::wstring emissionNode(L"emission");
		const std::wstring ambientNode(L"ambient");
		const std::wstring diffuseNode(L"diffuse");
		const std::wstring specularNode(L"specular");
		const std::wstring specularLevel(L"spec_level");
		const std::wstring specularLevelH(L"specularLevel");
		const std::wstring shininessNode(L"shininess");
		const std::wstring reflectiveNode(L"reflective");
		const std::wstring reflectivityNode(L"reflectivity");
		const std::wstring transparentNode(L"transparent");
		const std::wstring transparencyNode(L"transparency");
		const std::wstring indexOfRefractionNode(L"index_of_refraction");
		const std::wstring effectSectionName(L"effect");
		const std::wstring profileCOMMONSectionName(L"profile_COMMON");
		const std::wstring techniqueNodeName(L"technique");
		const std::wstring colorNodeName(L"color");
		const std::wstring floatNodeName(L"float");
		const std::wstring textureNodeName(L"texture");
		const std::wstring extraNodeName(L"extra");
		const std::wstring newParamName(L"newparam");
		const std::wstring initFromNode(L"init_from");
		const std::wstring sourceNode(L"source");
		const std::wstring libraryEffect(L"library_effects");

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring nodeName = xmlRead->getNodeName();

				if (nodeName == profileCOMMONSectionName || nodeName == techniqueNodeName)
				{
					parseEffectNode(xmlRead, effect);
				}
				else if (nodeName == newParamName)
				{
					int m_readState = 0;

					SEffectParam effectParam;

					if (xmlRead->getAttributeValue(L"sid"))
						effectParam.Name = xmlRead->getAttributeValue(L"sid");

					while (xmlRead->read())
					{
						std::wstring node = xmlRead->getNodeName();

						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							if (node == initFromNode)
								m_readState = 1;
							else if (node == sourceNode)
								m_readState = 2;

						}
						else if (xmlRead->getNodeType() == io::EXN_TEXT)
						{
							if (m_readState == 1)
								effectParam.InitFromTexture = xmlRead->getNodeData();
							else if (m_readState == 2)
								effectParam.Source = xmlRead->getNodeData();

							m_readState = 0;
						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (node == newParamName)
								break;
						}
					}

					// add to list effect params
					effect->EffectParams.push_back(effectParam);

				}
				else if (constantNode == nodeName || lambertNode == nodeName || phongNode == nodeName || blinnNode == nodeName)
				{
					if (xmlRead->isEmptyElement() == false)
					{
						// set phong shading
						effect->Mat.setFlag(irr::video::EMF_GOURAUD_SHADING, phongNode == nodeName || blinnNode == nodeName);

						// read color type
						while (xmlRead->read())
						{
							std::wstring node = xmlRead->getNodeName();

							if (xmlRead->getNodeType() == io::EXN_ELEMENT)
							{
								if (emissionNode == node || ambientNode == node || diffuseNode == node ||
									specularNode == node || reflectiveNode == node || transparentNode == node)
								{

									// read transparent type
									if (node == transparentNode)
									{
										if (xmlRead->getAttributeValue(L"opaque"))
										{
											if (std::wstring(L"RGB_ZERO") == xmlRead->getAttributeValue(L"opaque") ||
												std::wstring(L"A_ONE") == xmlRead->getAttributeValue(L"opaque"))
											{
												effect->TransparentAddColor = false;
											}
										}
									}

									// read color
									while (xmlRead->read())
									{
										if (xmlRead->getNodeType() == io::EXN_ELEMENT)
										{
											if (colorNodeName == xmlRead->getNodeName())
											{
												const video::SColorf colorf = readColorNode(xmlRead);
												const video::SColor color = colorf.toSColor();

												if (emissionNode == node)
													effect->Mat.EmissiveColor = color;
												else if (ambientNode == node)
													effect->Mat.AmbientColor = color;
												else if (diffuseNode == node)
													effect->Mat.DiffuseColor = color;
												else if (specularNode == node)
													effect->Mat.SpecularColor = color;
												else if (transparentNode == node)
												{
													effect->Transparency = colorf.getAlpha();
													if (effect->Transparency != 1.0f)
														effect->HasAlpha = true;
												}
											}
											else if (textureNodeName == xmlRead->getNodeName())
											{
												if (node == diffuseNode)
												{
													std::wstring tname = xmlRead->getAttributeValue(L"texture");
													if (tname.empty() == false)
													{
														effect->HasDiffuseMapping = true;
														effect->DiffuseRef = tname;
													}
												}
												else if (node == ambientNode)
												{
													// ambient lightmap texture:
													// see lightmap component
												}
												else if (node == specularNode)
												{
													// specular
													std::wstring tname = xmlRead->getAttributeValue(L"texture");
													if (tname.empty() == false)
													{
														effect->HasSpecularMapping = true;
														effect->SpecularRef = tname;
													}
												}
												else if (node == bumpNode)
												{
													// specular
													std::wstring tname = xmlRead->getAttributeValue(L"texture");
													if (tname.empty() == false)
													{
														effect->HasBumpMapping = true;
														effect->BumpRef = tname;
													}
												}
												else if (node == transparentNode)
												{
													// alpha node
													effect->HasAlpha = true;
												}
											}
										}
										else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
										{
											if (node == xmlRead->getNodeName())
												break;
										}
									}
								}
								else if (shininessNode == node || reflectivityNode == node || transparencyNode == node || indexOfRefractionNode == node)
								{
									// float or param types
									while (xmlRead->read())
									{
										if (xmlRead->getNodeType() == io::EXN_ELEMENT)
										{
											if (floatNodeName == xmlRead->getNodeName())
											{
												f32 f = readFloatNode(xmlRead);
												if (shininessNode == node)
													effect->Mat.Shininess = f;
												else if (transparencyNode == node)
													effect->Transparency *= f;
											}
										}
										else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
										{
											if (node == xmlRead->getNodeName())
												break;
										}
									}
								}
							}
							else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
							{
								if (constantNode == node ||
									lambertNode == node ||
									phongNode == node ||
									blinnNode == node ||
									bumpNode == node)
									break;
							}
						}
					}
				}
				else if (specularLevel == nodeName || specularLevelH == nodeName)
				{
					// <extra><spec_level> ... </spec_level></extra>
					while (xmlRead->read())
					{
						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							std::wstring node = xmlRead->getNodeName();

							if (textureNodeName == xmlRead->getNodeName())
							{
								std::wstring tname = xmlRead->getAttributeValue(L"texture");
								if (tname.empty() == false)
								{
									effect->HasSpecularMapping = true;
									effect->SpecularRef = tname;
								}
							}

						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (specularLevel == xmlRead->getNodeName() || specularLevelH == nodeName)
								break;
						}
					}

				}
				else if (bumpNode == nodeName)
				{
					// <extra><bump> ... </bump></extra>
					if (m_loadNormalMap)
					{
						while (xmlRead->read())
						{
							if (xmlRead->getNodeType() == io::EXN_ELEMENT)
							{
								std::wstring node = xmlRead->getNodeName();

								if (textureNodeName == xmlRead->getNodeName())
								{
									std::wstring tname = xmlRead->getAttributeValue(L"texture");
									if (tname.empty() == false)
									{
										effect->HasBumpMapping = true;
										effect->BumpRef = tname;
									}
								}
							}
							else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
							{
								if (bumpNode == xmlRead->getNodeName())
									break;
							}
						}
					}
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				std::wstring nodeName = xmlRead->getNodeName();

				if (effectSectionName == nodeName)
					break;
				else if (profileCOMMONSectionName == nodeName)
					break;
				else if (techniqueNodeName == nodeName)
					break;
				else if (libraryEffect == nodeName)
					break;
			}
		}

		if (effect)
			updateEffectMaterial(effect);
	}

	void CColladaLoader::parseGeometryNode(io::IXMLReader* xmlRead)
	{
		const std::wstring geometrySectionName(L"geometry");
		const std::wstring sourceNode(L"source");
		const std::wstring verticesNode(L"vertices");
		const std::wstring vcountNode(L"vcount");
		const std::wstring trianglesNode(L"triangles");
		const std::wstring polylistNode(L"polylist");
		const std::wstring polygonsNode(L"polygons");
		const std::wstring floatArrayNode(L"float_array");
		const std::wstring inputNode(L"input");
		const std::wstring accessorNode(L"accessor");

		SMeshParam mesh;

		mesh.Type = k_mesh;
		mesh.Name = readId(xmlRead);

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring nodeName = xmlRead->getNodeName();

				//<source>
				if (nodeName == sourceNode)
				{
					SBufferParam buffer;

					buffer.Name = readId(xmlRead);
					buffer.Type = k_positionBuffer;
					buffer.ArrayCount = 0;
					buffer.Strike = 0;
					buffer.FloatArray = NULL;

					while (xmlRead->read())
					{
						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							if (xmlRead->getNodeName() == floatArrayNode)
							{
								buffer.ArrayCount = xmlRead->getAttributeValueAsInt(L"count");
								buffer.FloatArray = new float[buffer.ArrayCount];

								readFloatsInsideElement(xmlRead, buffer.FloatArray, buffer.ArrayCount);
							}
							else if (xmlRead->getNodeName() == accessorNode)
							{
								buffer.Strike = xmlRead->getAttributeValueAsInt(L"stride");
							}
						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (xmlRead->getNodeName() == sourceNode)
								break;
						}
					}

					mesh.Buffers.push_back(buffer);

				}
				//<vertices>
				else if (nodeName == verticesNode)
				{
					SVerticesParam	verticesParam;
					verticesParam.Name = readId(xmlRead);

					while (xmlRead->read())
					{
						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							if (xmlRead->getNodeName() == inputNode)
							{
								std::wstring semantic = xmlRead->getAttributeValue(L"semantic");
								std::wstring source = xmlRead->getAttributeValue(L"source");
								source.erase(source.begin());

								int bufferID = getBufferWithUri(source, &mesh);

								if (bufferID != -1)
								{
									SBufferParam* buffer = &mesh.Buffers[bufferID];

									if (semantic == L"POSITION")
									{
										buffer->Type = k_positionBuffer;
										verticesParam.PositionIndex = bufferID;
									}
									else if (semantic == L"NORMAL")
									{
										buffer->Type = k_normalBuffer;
										verticesParam.NormalIndex = bufferID;
									}
									else if (semantic == L"TEXCOORD")
									{
										buffer->Type = k_texCoordBuffer;

										int set = xmlRead->getAttributeValueAsInt(L"set");

										if (set == 0)
										{
											verticesParam.TexCoord1Index = bufferID;
										}
										else if (set == 1)
										{
											verticesParam.TexCoord2Index = bufferID;
										}
									}
								}

							}
						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (xmlRead->getNodeName() == verticesNode)
								break;
						}
					}

					// add vertex param
					mesh.Vertices.push_back(verticesParam);
				}
				//<triangles>
				//<polylistNode>
				else if (nodeName == trianglesNode || nodeName == polylistNode)
				{
					STrianglesParam triangle;

					triangle.NumElementPerVertex = 0;

					triangle.NumPolygon = xmlRead->getAttributeValueAsInt(L"count");

					if (xmlRead->getAttributeValue(L"material"))
					{
						std::wstring materialName = xmlRead->getAttributeValue(L"material");
						triangle.Material = materialName;
					}

					while (xmlRead->read())
					{
						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							if (xmlRead->getNodeName() == inputNode)
							{
								std::wstring source = xmlRead->getAttributeValue(L"source");
								std::wstring semantic = xmlRead->getAttributeValue(L"semantic");

								int offset = 0;
								offset = xmlRead->getAttributeValueAsInt(L"offset");

								source.erase(source.begin());

								if (semantic == L"NORMAL")
								{
									int bufferID = getBufferWithUri(source, &mesh);
									mesh.Buffers[bufferID].Type = k_normalBuffer;

									if (triangle.VerticesIndex != -1)
									{
										mesh.Vertices[triangle.VerticesIndex].NormalIndex = bufferID;
										triangle.OffsetNormal = offset;
									}

									triangle.NumElementPerVertex++;
								}
								else if (semantic == L"TEXCOORD")
								{
									int bufferID = getBufferWithUri(source, &mesh);
									mesh.Buffers[bufferID].Type = k_texCoordBuffer;
									if (triangle.VerticesIndex != -1)
									{
										SVerticesParam& verticesParam = mesh.Vertices[triangle.VerticesIndex];
										int set = xmlRead->getAttributeValueAsInt(L"set");

										if (set == 0)
										{
											verticesParam.TexCoord1Index = bufferID;
											triangle.OffsetTexcoord1 = offset;
										}
										else if (set == 1)
										{
											verticesParam.TexCoord2Index = bufferID;
											triangle.OffsetTexcoord2 = offset;
										}
									}

									triangle.NumElementPerVertex++;
								}
								else if (semantic == L"VERTEX")
								{
									triangle.VerticesIndex = getVerticesWithUri(source, &mesh);
									triangle.OffsetVertex = offset;

									triangle.NumElementPerVertex++;
								}
								else
								{
									triangle.NumElementPerVertex++;
								}
							}
							else if (xmlRead->getNodeName() == vcountNode)
							{
								//<vcount>
								triangle.VCount = new s32[triangle.NumPolygon];
								readIntsInsideElement(xmlRead, triangle.VCount, triangle.NumPolygon);
							}
							else if (xmlRead->getNodeName() == std::wstring(L"p")
								&& triangle.VerticesIndex != -1
								&& triangle.NumElementPerVertex > 0)
							{
								if (nodeName == polylistNode)
								{
									std::vector<s32> indexBuffer;

									std::vector<s32> arrayInt;
									readIntsInsideElement(xmlRead, arrayInt);
									int sourceID = 0;
									int targetID = 0;

									// num triangle
									int numTris = calcNumTriangleFromPolygon(triangle.VCount, triangle.NumPolygon);

									// polylist -> triangle
									triangle.IndexBuffer = new s32[numTris * triangle.NumElementPerVertex * 3];

									for (int i = 0; i < triangle.NumPolygon; i++)
									{
										sourceID = convertPolygonToTriangle(
											arrayInt.data(), sourceID,			// source polygon
											triangle.IndexBuffer, targetID,		// target triangle
											triangle.VCount[i],					// vertex count
											triangle.NumElementPerVertex		// element per vertex
										);
									}

									triangle.NumPolygon = numTris;
								}
								else
								{
									// triangles
									triangle.IndexBuffer = new s32[triangle.NumPolygon * triangle.NumElementPerVertex * 3];
									readIntsInsideElement(xmlRead, triangle.IndexBuffer, triangle.NumPolygon * triangle.NumElementPerVertex * 3);
								}
							}
						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (xmlRead->getNodeName() == trianglesNode || xmlRead->getNodeName() == polylistNode)
								break;
						}
					}

					// delete vcount (no need to use)
					if (triangle.VCount != NULL)
					{
						delete triangle.VCount;
						triangle.VCount = NULL;
					}

					// add triangles
					if (triangle.IndexBuffer != NULL)
						mesh.Triangles.push_back(triangle);
				}
				//<polygons>
				else if (nodeName == polygonsNode)
				{
					char log[1024];
					char nodeNameA[256];
					CStringImp::copy(nodeNameA, nodeName.c_str());
					sprintf(log, "[Collada loader] unsupport %s - reimport <triangles> or <polylist>", nodeNameA);
					os::Printer::log(log);
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (xmlRead->getNodeName() == geometrySectionName)
					break;
			}
		}

		m_listMesh.push_back(mesh);
	}

	void CColladaLoader::parseControllerNode(io::IXMLReader* xmlRead)
	{
		const std::wstring skinSectionName(L"skin");
		const std::wstring controllerSectionName(L"controller");

		std::wstring controllerName = xmlRead->getAttributeValue(L"id");

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring node = xmlRead->getNodeName();

				if (node == skinSectionName)
				{
					SMeshParam* mesh = parseSkinNode(xmlRead);

					if (mesh)
						mesh->ControllerName = controllerName;
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (xmlRead->getNodeName() == controllerSectionName)
					break;
			}
		}
	}

	SMeshParam* CColladaLoader::parseSkinNode(io::IXMLReader* xmlRead)
	{
		std::wstring source = xmlRead->getAttributeValue(L"source");
		source.erase(source.begin());

		int meshID = getMeshWithUri(source, m_listMesh);
		if (meshID == -1)
			return NULL;

		SMeshParam* mesh = &m_listMesh[meshID];

		const std::wstring paramSectionName(L"param");
		const std::wstring skinSectionName(L"skin");
		const std::wstring jointsSectionName(L"joints");
		const std::wstring weightsSectionName(L"vertex_weights");
		const std::wstring nameArraySectionName(L"Name_array");
		const std::wstring floatArraySectionName(L"float_array");
		const std::wstring intArraySectionName(L"int_array");
		const std::wstring vcountNode(L"vcount");
		const std::wstring vNode(L"v");
		const std::wstring sourceNode(L"source");
		const std::wstring bindShapeMatrix(L"bind_shape_matrix");

		int numJoints;
		std::wstring jointsName;

		int numArray = 0;
		float* transformArray = NULL;
		float* weightArray = NULL;

		std::vector<s32>& vCountArray = mesh->JointVertexIndex;
		std::vector<s32> vArray;

		while (xmlRead->read())
		{
			if (xmlRead->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring node = xmlRead->getNodeName();

				if (node == bindShapeMatrix)
				{
					float f[16];
					readFloatsInsideElement(xmlRead, f, 16);

					core::matrix4 mat;
					mat.setM(f);
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

						mesh->BindShapeMatrix = mat2;
					}
					else
					{
						mesh->BindShapeMatrix = mat.getTransposed();
					}
				}
				// <source>
				else if (node == sourceNode)
				{
					float* f = NULL;

					std::wstring sourceId = xmlRead->getAttributeValue(L"id");

					while (xmlRead->read())
					{
						if (xmlRead->getNodeType() == io::EXN_ELEMENT)
						{
							// <Name_array>
							if (xmlRead->getNodeName() == nameArraySectionName)
							{
								numJoints = xmlRead->getAttributeValueAsInt(L"count");
								readStringInsideElement(xmlRead, jointsName);
							}
							// <float_array>
							else if (xmlRead->getNodeName() == floatArraySectionName)
							{
								int count = xmlRead->getAttributeValueAsInt(L"count");
								numArray = count;
								f = new float[count];
								readFloatsInsideElement(xmlRead, f, count);
							}
							// <param> inside <accessor>
							else if (xmlRead->getNodeName() == paramSectionName)
							{
								if (xmlRead->getAttributeValue(L"name") != NULL)
								{
									std::wstring name = xmlRead->getAttributeValue(L"name");
									if (name == L"TRANSFORM")
										transformArray = f;
									else if (name == L"WEIGHT")
										weightArray = f;
									else
									{
										// delete float buffer on another accessor
										if (f != NULL)
										{
											delete f;
											f = NULL;
										}
									}
								}
								else
								{
									if (sourceId.find(L"-Matrices") != std::wstring::npos)
										transformArray = f;
									else if (sourceId.find(L"-Weights") != std::wstring::npos)
										weightArray = f;
									else
									{
										// delete float buffer on another accessor
										if (f != NULL)
										{
											delete f;
											f = NULL;
										}
									}
								}
							}
						}
						else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
						{
							if (xmlRead->getNodeName() == sourceNode)
								break;
						}
					}
				}
				// <vcount>
				else if (node == vcountNode)
				{
					readIntsInsideElement(xmlRead, vCountArray);
				}
				// <vNode>
				else if (node == vNode)
				{
					readIntsInsideElement(xmlRead, vArray);
				}
			}
			else if (xmlRead->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (xmlRead->getNodeName() == skinSectionName)
					break;
			}
		}

		mesh->Type = k_skinMesh;
		mesh->ListJointName = jointsName;

		updateJointToMesh(mesh, numJoints, weightArray, transformArray, vCountArray, vArray);

		if (weightArray)
			delete weightArray;

		if (transformArray)
			delete transformArray;

		return mesh;
	}

	void CColladaLoader::updateJointToMesh(SMeshParam* mesh,
		int numJoints,
		float* arrayWeight,
		float* arrayTransform,
		std::vector<s32>& vCountArray,
		std::vector<s32>& vArray)
	{
		// get array float
		f32* f = (f32*)arrayTransform;

		for (int i = 0; i < numJoints; i++)
		{
			SJointParam newJoint;

			core::matrix4 mat;
			mat.setM(f + i * 16);

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

				newJoint.InvMatrix = mat2;
			}
			else
			{
				newJoint.InvMatrix = mat.getTransposed();
			}

			// add joint to controller
			mesh->Joints.push_back(newJoint);
		}

		// set vertex weight
		int nVertex = (int)vCountArray.size();
		int id = 0;

		for (int i = 0; i < nVertex; i++)
		{
			// num of bone in vertex
			int nBone = vCountArray[i];

			// loop on bone in vertex
			for (int iBone = 0; iBone < nBone; iBone++, id += 2)
			{
				u32 boneId = vArray[id];
				u32 weightId = vArray[id + 1];
				f32 f = arrayWeight[weightId];

				SWeightParam weightParam;

				weightParam.VertexID = i;
				weightParam.Strength = f;

				// add weight on bone
				mesh->Joints[boneId].Weights.push_back(weightParam);

				mesh->JointIndex.push_back(boneId);
				mesh->JointIndex.push_back((int)mesh->Joints[boneId].Weights.size() - 1);
			}

		}
	}

	void CColladaLoader::parseSceneNode(io::IXMLReader* xmlRead)
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

	SNodeParam* CColladaLoader::parseNode(io::IXMLReader* xmlRead, SNodeParam* parent)
	{
		const std::wstring nodeSectionName(L"node");
		const std::wstring translateSectionName(L"translate");
		const std::wstring rotateSectionName(L"rotate");
		const std::wstring scaleSectionName(L"scale");
		const std::wstring matrixNodeName(L"matrix");

		const std::wstring instanceGeometrySectionName(L"instance_geometry");
		const std::wstring instanceControllerSectionName(L"instance_controller");
		const std::wstring instanceMaterialSectionName(L"instance_material");

		SNodeParam* pNode = new SNodeParam();

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

			wchar_t namew[512];
			CStringImp::convertUTF8ToUnicode(m_meshName.c_str(), namew);

			m_colladaRoot->Name = namew;
			m_colladaRoot->SID = L"Root";
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

			// unit scale on render mesh node
			if (m_unit != "meter")
			{
				core::matrix4 scale;
				scale.setScale(m_unitScale);
				m_unitScaleMatrix.setScale(m_unitScale);

				m_colladaRoot->Transform *= m_unitScaleMatrix;
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
					// mull matix
					pNode->Transform *= readMatrixNode(xmlRead, m_zUp);
				}
				else if (xmlRead->getNodeName() == instanceGeometrySectionName)
				{
					// <instance_geometry url="#MESHNAME">
					pNode->Instance = xmlRead->getAttributeValue(L"url");
					uriToId(pNode->Instance);
				}
				else if (xmlRead->getNodeName() == instanceMaterialSectionName)
				{
					//<instance_material symbol="#MESHMATERIAL" target="#MATERIAL">
					std::wstring symbol = xmlRead->getAttributeValue(L"symbol");
					std::wstring target = xmlRead->getAttributeValue(L"target");
					uriToId(target);

					pNode->BindMaterial[symbol] = target;
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

	void CColladaLoader::updateEffectMaterial(SEffect* effect)
	{
		if (effect->HasAlpha == true)
		{
			if (effect->Transparency != 1.0f && effect->TransparentAddColor == false)
			{
				// todo add material id
			}
			else
			{
				if (effect->TransparentAddColor)
				{
					// todo add material id
				}
				else
				{
					// todo add material id
				}
			}

			effect->Mat.Lighting = false;
			effect->Mat.BackfaceCulling = false;
			effect->Mat.FrontfaceCulling = false;
		}
		else
		{
			if (effect->HasSpecularMapping)
			{
				// todo add material id
			}
			if (effect->HasBumpMapping)
			{
				// todo add material id
			}
			if (effect->HasBumpMapping && effect->HasSpecularMapping)
			{
				// todo add material id
			}
		}

		// add mipmap filter
		effect->Mat.UseMipMaps = true;
		effect->Mat.setFlag(video::EMF_BILINEAR_FILTER, false);
		effect->Mat.setFlag(video::EMF_TRILINEAR_FILTER, false);
		effect->Mat.setFlag(video::EMF_ANISOTROPIC_FILTER, true, 2);
	}

	ITexture* CColladaLoader::getTextureResource(std::wstring& refName, ArrayEffectParams& params)
	{
		ITexture* tex = getTextureFromImage(refName, params, m_listImages, m_textureFolder);
		if (tex == NULL)
			tex = getTextureFromImage(refName, m_listImages, m_textureFolder);

		return tex;
	}

	void CColladaLoader::loadEffectTexture()
	{
		for (u32 i = 0, n = (u32)m_listEffects.size(); i < n; i++)
		{
			SEffect& effect = m_listEffects[i];
			ITexture* tex = NULL;

			// load diffuse
			if (effect.HasDiffuseMapping == true)
			{
				tex = getTextureResource(effect.DiffuseRef, effect.EffectParams);
				if (tex != NULL)
					effect.Mat.setTexture(0, tex);
				else
					effect.HasDiffuseMapping = false;
			}

			// load specular
			if (effect.HasSpecularMapping == true)
			{
				tex = getTextureResource(effect.SpecularRef, effect.EffectParams);
				if (tex != NULL)
					effect.Mat.setTexture(1, tex);
				else
					effect.HasSpecularMapping = false;
			}

			// load normal map
			if (effect.HasBumpMapping == true)
			{
				if (m_loadNormalMap == true)
				{
					tex = getTextureResource(effect.BumpRef, effect.EffectParams);
					if (tex != NULL)
						effect.Mat.setTexture(2, tex);
					else
						effect.HasBumpMapping = false;
				}
				else
				{
					effect.HasBumpMapping = false;
				}
			}
		}
	}

	void CColladaLoader::constructEntityPrefab(CEntityPrefab* output)
	{
		std::list<SNodeParam*> stackColladaNodes;
		std::list<SNodeParam*> listColladaNodes;

		int nNode = (int)m_listNode.size();
		for (int i = 0; i < nNode; i++)
		{
			SNodeParam* root = m_listNode[i];
			stackColladaNodes.push_back(root);
		}

		char name[512] = { 0 };

		while (stackColladaNodes.size())
		{
			SNodeParam* node = stackColladaNodes.back();
			listColladaNodes.push_back(node);

			// find parent entity
			CEntity* parent = NULL;
			if (node->Parent && node->Parent->Entity)
				parent = node->Parent->Entity;

			// get parent
			CWorldTransformData* parentTransform = NULL;
			if (parent != NULL)
				parentTransform = GET_ENTITY_DATA(parent, CWorldTransformData);

			// create entity
			CEntity* entity = output->createEntity();

			// tag this entity
			node->Entity = entity;

			// add transform
			core::matrix4 relativeTransform = node->Transform;

			// add entity with transform
			CStringImp::copy(name, node->Name.c_str());
			output->addTransformData(entity, parent, relativeTransform, name);

			// add joint data if this node is JOINT
			if (node->Type == L"JOINT")
			{
				CJointData* jointData = entity->addData<CJointData>();
				jointData->BoneName = name;

				if (node->ChildLevel == 0)
					jointData->BoneRoot = true;

				if (node->SID.size() > 0)
				{
					CStringImp::copy(name, node->SID.c_str());
					jointData->SID = name;
				}

				// store for next construct skinned mesh
				m_nameToJointData[jointData->BoneName] = jointData;
				m_sidToJointData[jointData->SID] = jointData;
			}

			// calc world transform
			CWorldTransformData* transformData = GET_ENTITY_DATA(entity, CWorldTransformData);
			if (parentTransform != NULL)
				transformData->World.setbyproduct_nocheck(parentTransform->World, transformData->Relative);
			else
				transformData->World = transformData->Relative;

			// construct geometry & controller in node
			if (node->Instance.size() > 0)
			{
				int meshID = getMeshWithUri(node->Instance, m_listMesh);
				if (meshID == -1)
					meshID = getMeshWithControllerName(node->Instance, m_listMesh);

				if (meshID != -1)
				{
					SMeshParam* pMesh = &m_listMesh[meshID];

					CMesh* mesh = constructMesh(pMesh, node);
					if (mesh)
					{
						// add & clone new render mesh
						CRenderMeshData* meshData = entity->addData<CRenderMeshData>();
						meshData->setMesh(mesh);

						// drop this mesh
						mesh->drop();

						// add culling data
						entity->addData<CCullingData>();
					}
				}
			}

			// pop stack
			stackColladaNodes.erase(--stackColladaNodes.end());

			// add child to continue loop
			int nChild = (int)node->Childs.size();
			for (int i = 0; i < nChild; i++)
			{
				SNodeParam* childNode = node->Childs[i];

				// set parent
				childNode->Parent = node;
				stackColladaNodes.push_back(childNode);
			}
		}

		// construct apply bone weight to skinned mesh
		constructSkinMesh(listColladaNodes);
	}

	void CColladaLoader::constructSkinMesh(std::list<SNodeParam*>& nodes)
	{
		for (SNodeParam*& node : nodes)
		{
			CRenderMeshData* renderMesh = GET_ENTITY_DATA(node->Entity, CRenderMeshData);
			if (renderMesh != NULL)
			{
				CSkinnedMesh* skinnedMesh = dynamic_cast<CSkinnedMesh*>(renderMesh->getMesh());
				if (skinnedMesh != NULL)
				{
					int meshID = getMeshWithControllerName(node->Instance, m_listMesh);
					constructSkinMesh(&m_listMesh[meshID], skinnedMesh);

					// add software skinning
#if defined(IOS_SIMULATOR)
					renderMesh->setSoftwareSkinning(true);
#else
					if (skinnedMesh->Joints.size() >= GPU_BONES_COUNT)
						renderMesh->setSoftwareSkinning(true);
#endif
					renderMesh->setSkinnedMesh(true);
				}
			}
		}
	}

	CMesh* CColladaLoader::constructMesh(SMeshParam* mesh, SNodeParam* node)
	{
		CMesh* colladaMesh = NULL;

		bool staticMesh = mesh->Type == k_mesh;

		if (staticMesh == true)
			colladaMesh = new CMesh();
		else
			colladaMesh = new CSkinnedMesh();

		// add mesh buffer
		int numBuffer = (int)mesh->Triangles.size();
		for (int i = 0; i < numBuffer; i++)
		{
			STrianglesParam& tri = mesh->Triangles[i];
			if (tri.NumPolygon == 0)
				continue;

			// bind material
			if (tri.Material.empty() == false)
			{
				std::wstring bindMaterial = node->BindMaterial[tri.Material];
				if (bindMaterial.empty() == false)
					tri.EffectIndex = getEffectWithUri(bindMaterial, m_listEffects, m_listMaterial);
			}

			// get effect
			SEffect* effect = NULL;
			if (tri.EffectIndex != -1)
				effect = &m_listEffects[tri.EffectIndex];

			bool needFixUV = false;
			IMeshBuffer* buffer = NULL;
			buffer = constructMeshBuffer(mesh, &tri, i, needFixUV);

			if (staticMesh == true)
			{
				// STATIC MESH BUFFER
				if (m_loadTexcoord2 == true)
				{
					if (mesh->Vertices[tri.VerticesIndex].TexCoord2Index > 0)
					{
						convertToLightMapVertices(buffer, mesh, &tri);
					}
				}
				else if ((effect && effect->HasBumpMapping) || m_createTangent)
				{
					CMeshUtils::convertToTangentVertices(buffer, m_flipNormalMap);
				}
			}
			else
			{
				// BIND SHAPE MATRIX
				CSkinnedMesh* skinnedMesh = (CSkinnedMesh*)colladaMesh;

				// SKIN MESH BUFFER
				if (m_createTangent)
				{
					// construct skin tangent buffer
					CMeshUtils::convertToSkinTangentVertices(buffer, m_flipNormalMap);
				}
				else
				{
					// construct skin buffer
					CMeshUtils::convertToSkinVertices(buffer);
				}
			}

			//if (needFixUV && s_fixUV)
			//	fixUVTitle(buffer);

			char materialName[512];
			strcpy(materialName, "");

			if (effect != NULL)
				CStringImp::copy(materialName, effect->Id.c_str());

			if (buffer)
			{
				colladaMesh->addMeshBuffer(buffer, materialName);
				buffer->drop();
			}
		}

		// calc bbox
		colladaMesh->recalculateBoundingBox();

		// notify create static hardware buffer
		colladaMesh->setHardwareMappingHint(EHM_STATIC);

		return colladaMesh;
	}

	IMeshBuffer* CColladaLoader::constructMeshBuffer(SMeshParam* mesh, STrianglesParam* tri, int bufferID, bool& needFixUVTile)
	{
		E_INDEX_TYPE indexBufferType = video::EIT_16BIT;
		int numVertex = tri->NumPolygon * 3;
		if (numVertex >= USHRT_MAX - 1)
			indexBufferType = video::EIT_32BIT;

		CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(
			getVideoDriver()->getVertexDescriptor(video::EVT_STANDARD),
			indexBufferType);

		IVertexBuffer* vertexBuffer = buffer->getVertexBuffer();
		IIndexBuffer* indexBuffer = buffer->getIndexBuffer();

		SVerticesParam* vertices = &mesh->Vertices[tri->VerticesIndex];

		SEffect* effect = NULL;
		SBufferParam* position = NULL;
		SBufferParam* normal = NULL;
		SBufferParam* texCoord1 = NULL;
		SBufferParam* texCoord2 = NULL;

		needFixUVTile = false;

		if (tri->EffectIndex != -1)
			effect = &m_listEffects[tri->EffectIndex];

		if (vertices->PositionIndex == -1)
			return NULL;

		// position buffer
		position = &mesh->Buffers[vertices->PositionIndex];

		if (vertices->NormalIndex != -1)
			normal = &mesh->Buffers[vertices->NormalIndex];

		if (vertices->TexCoord1Index != -1)
			texCoord1 = &mesh->Buffers[vertices->TexCoord1Index];

		if (vertices->TexCoord2Index != -1)
			texCoord2 = &mesh->Buffers[vertices->TexCoord2Index];

		// index buffer
		core::array<u32> indices;

		// if have the normal & texcoord
		if (tri->NumElementPerVertex == 1)
		{
			// alloc number of vertex
			int vertexCount = position->ArrayCount / 3;

			// add to vertex buffer
			for (int i = 0; i < vertexCount; i++)
			{
				video::S3DVertex vtx;
				vtx.Color = SColor(0xFFFFFFFF);

				int vIndex = i;
				int idx = vIndex * position->Strike;

				// set position
				vtx.Pos.X = position->FloatArray[idx + 0];
				if (m_zUp)
				{
					vtx.Pos.Z = position->FloatArray[idx + 1];
					vtx.Pos.Y = position->FloatArray[idx + 2];
				}
				else
				{
					vtx.Pos.Y = position->FloatArray[idx + 1];
					vtx.Pos.Z = position->FloatArray[idx + 2];
				}

				// set normal
				if (normal != NULL)
				{
					int idx = vIndex * normal->Strike;

					vtx.Normal.X = normal->FloatArray[idx + 0];
					if (m_zUp)
					{
						vtx.Normal.Z = normal->FloatArray[idx + 1];
						vtx.Normal.Y = normal->FloatArray[idx + 2];
					}
					else
					{
						vtx.Normal.Y = normal->FloatArray[idx + 1];
						vtx.Normal.Z = normal->FloatArray[idx + 2];
					}

					vtx.Normal.normalize();
				}

				// set texcoord
				if (texCoord1 != NULL)
				{
					idx = vIndex * texCoord1->Strike;
					if (texCoord1 != NULL)
					{
						vtx.TCoords.X = texCoord1->FloatArray[idx + 0];
						vtx.TCoords.Y = texCoord1->FloatArray[idx + 1];

						if (vtx.TCoords.X > m_maxUVTile ||
							vtx.TCoords.Y > m_maxUVTile ||
							vtx.TCoords.X < -m_maxUVTile ||
							vtx.TCoords.Y < -m_maxUVTile)
							needFixUVTile = true;
					}
				}

				// add to vertex buffer
				vertexBuffer->addVertex(&vtx);

				u32 newIndex = vertexBuffer->getVertexCount() - 1;

				if (mesh->Type == k_skinMesh)
				{
					// apply mesh map (for affect bone)
					SColladaMeshVertexMap map;
					map.meshId = mesh;
					map.bufferId = bufferID;
					map.vertexId = vIndex;
					m_meshVertexIndex[map].push_back(newIndex);
				}
			}

			int totalElement = tri->NumPolygon * tri->NumElementPerVertex * 3;
			int index = 0;
			for (int i = 0; i < totalElement; i += tri->NumElementPerVertex)
			{
				// add to index buffer
				indices.push_back(tri->IndexBuffer[i]);
				index++;
			}

		}
		else
		{
			// FBX to DAE converter
			int totalElement = tri->NumPolygon * tri->NumElementPerVertex * 3;
			int index = 0;

			SColladaVertexIndex	vertexIndex;

			for (int i = 0; i < totalElement; i += tri->NumElementPerVertex)
			{
				// reset collada index
				vertexIndex.vertexId = tri->IndexBuffer[i] * position->Strike;
				vertexIndex.normalId = -1;
				vertexIndex.texcoordId1 = -1;
				vertexIndex.texcoordId2 = -1;

				// sync texcoord & position
				if (texCoord1 != NULL)
					vertexIndex.texcoordId1 = tri->IndexBuffer[i + tri->OffsetTexcoord1] * texCoord1->Strike;

				if (normal != NULL)
					vertexIndex.normalId = tri->IndexBuffer[i + tri->OffsetNormal] * normal->Strike;

				// new vertex infomation
				video::S3DVertex vtx;
				vtx.Color = SColor(0xFFFFFFFF);

				// set position
				int idx = vertexIndex.vertexId;

				vtx.Pos.X = position->FloatArray[idx];
				if (m_zUp)
				{
					vtx.Pos.Z = position->FloatArray[idx + 1];
					vtx.Pos.Y = position->FloatArray[idx + 2];
				}
				else
				{
					vtx.Pos.Y = position->FloatArray[idx + 1];
					vtx.Pos.Z = position->FloatArray[idx + 2];
				}

				// set normal
				if (vertexIndex.normalId != -1)
				{
					idx = vertexIndex.normalId;
					vtx.Normal.X = normal->FloatArray[idx + 0];
					if (m_zUp)
					{
						vtx.Normal.Z = normal->FloatArray[idx + 1];
						vtx.Normal.Y = normal->FloatArray[idx + 2];
					}
					else
					{
						vtx.Normal.Y = normal->FloatArray[idx + 1];
						vtx.Normal.Z = normal->FloatArray[idx + 2];
					}

					vtx.Normal.normalize();
				}

				// set texcoord
				if (vertexIndex.texcoordId1 != -1)
				{
					idx = vertexIndex.texcoordId1;
					vtx.TCoords.X = texCoord1->FloatArray[idx + 0];
					vtx.TCoords.Y = 1.0f - texCoord1->FloatArray[idx + 1];

					if (vtx.TCoords.X > m_maxUVTile ||
						vtx.TCoords.Y > m_maxUVTile ||
						vtx.TCoords.X < -m_maxUVTile ||
						vtx.TCoords.Y < -m_maxUVTile)
						needFixUVTile = true;
				}

				// add vertex buffer
				vertexBuffer->addVertex(&vtx);
				u32 newIndex = vertexBuffer->getVertexCount() - 1;

				// add index buffer
				indices.push_back(newIndex);
				index++;

				// FBX to collada
				// apply mesh map (for affect bone)
				if (mesh->Type == k_skinMesh)
				{
					SColladaMeshVertexMap map;
					map.meshId = mesh;
					map.bufferId = bufferID;
					map.vertexId = tri->IndexBuffer[i];
					m_meshVertexIndex[map].push_back(newIndex);
				}
			}
		}

		// it's just triangles
		u32 nPoly = indices.size() / 3;

		for (u32 i = 0; i < nPoly; i++)
		{
			u32 ind = i * 3;

			if (m_flipOx == true)
			{
				if (m_zUp)
				{
					indexBuffer->addIndex(indices[ind + 0]);
					indexBuffer->addIndex(indices[ind + 1]);
					indexBuffer->addIndex(indices[ind + 2]);
				}
				else
				{
					indexBuffer->addIndex(indices[ind + 2]);
					indexBuffer->addIndex(indices[ind + 1]);
					indexBuffer->addIndex(indices[ind + 0]);
				}
			}
			else
			{
				if (m_zUp)
				{
					indexBuffer->addIndex(indices[ind + 2]);
					indexBuffer->addIndex(indices[ind + 1]);
					indexBuffer->addIndex(indices[ind + 0]);
				}
				else
				{
					indexBuffer->addIndex(indices[ind + 0]);
					indexBuffer->addIndex(indices[ind + 1]);
					indexBuffer->addIndex(indices[ind + 2]);
				}
			}
		}

		if (!normal)
		{
			IMeshManipulator* meshManipulator = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();
			meshManipulator->recalculateNormals(buffer, true);

			if (m_flipOx)
			{
				IVertexBuffer* vertexBuffer = buffer->getVertexBuffer();
				video::S3DVertex* vertices = (video::S3DVertex*)vertexBuffer->getVertices();

				for (int i = 0, n = vertexBuffer->getVertexCount(); i < n; i++)
				{
					vertices[i].Normal *= -1.0f;
				}
			}
		}

		CShaderManager* shaderMgr = CShaderManager::getInstance();
		SMaterial& mat = buffer->getMaterial();

		if (effect)
		{
			mat = effect->Mat;

			if (effect->Mat.getTexture(0) != NULL)
				mat.MaterialType = shaderMgr->getShaderIDByName("TextureColor");
			else
				mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
		}
		else
		{
			mat.MaterialType = shaderMgr->getShaderIDByName("VertexColor");
		}

		buffer->recalculateBoundingBox();

		// apply unit scale for culling
		core::aabbox3df box = buffer->getBoundingBox();
		m_unitScaleMatrix.transformBox(box);
		buffer->getBoundingBox() = box;

		return buffer;
	}

	void CColladaLoader::convertToLightMapVertices(IMeshBuffer* buffer, SMeshParam* mesh, STrianglesParam* tri)
	{
		SVerticesParam* vertices = &mesh->Vertices[tri->VerticesIndex];

		SBufferParam* texCoord2 = NULL;

		if (vertices->TexCoord2Index != -1)
			texCoord2 = &mesh->Buffers[vertices->TexCoord2Index];

		buffer->setVertexDescriptor(getVideoDriver()->getVertexDescriptor(video::EVT_2TCOORDS));

		for (u32 i = 0; i < buffer->getVertexBufferCount(); ++i)
		{
			CVertexBuffer<video::S3DVertex2TCoords>* vertexBuffer = new CVertexBuffer<video::S3DVertex2TCoords>();

			// copy vertex data
			CMeshUtils::copyVertices(buffer->getVertexBuffer(i), vertexBuffer);

			// fill texcoord 2
			// if have the normal & texcoord
			if (tri->NumElementPerVertex == 1)
			{
				// DAE Exporter
				for (int i = 0, n = vertexBuffer->getVertexCount(); i < n; i++)
				{
					video::S3DVertex2TCoords& v = vertexBuffer->getVertex(i);

					int idx = i * texCoord2->Strike;
					if (texCoord2 != NULL)
					{
						v.TCoords2.X = texCoord2->FloatArray[idx + 0];
						v.TCoords2.Y = texCoord2->FloatArray[idx + 1];
					}
				}
			}
			else
			{
				// FBX to DAE
				int totalElement = tri->NumPolygon * tri->NumElementPerVertex * 3;

				int vID = 0;

				for (int i = 0; i < totalElement; i += tri->NumElementPerVertex)
				{
					video::S3DVertex2TCoords& v = vertexBuffer->getVertex(vID++);
					int texcoordId2 = tri->IndexBuffer[i + tri->OffsetTexcoord2] * texCoord2->Strike;

					// set texcoord
					v.TCoords2.X = texCoord2->FloatArray[texcoordId2 + 0];
					v.TCoords2.Y = 1.0f - texCoord2->FloatArray[texcoordId2 + 1];
				}
			}

			// replace the buffer
			buffer->setVertexBuffer(vertexBuffer, i);

			// drop reference
			vertexBuffer->drop();
		}
	}

	CJointData* CColladaLoader::findJointData(const char* name)
	{
		if (m_sidToJointData.find(name) != m_sidToJointData.end())
		{
			// found in SID
			return m_sidToJointData[name];
		}

		if (m_nameToJointData.find(name) != m_nameToJointData.end())
		{
			// found in Bone Name
			return m_nameToJointData[name];
		}

		return NULL;
	}

	void CColladaLoader::constructSkinMesh(SMeshParam* meshParam, CSkinnedMesh* mesh)
	{
		char name[1024];
		int nJoint = (int)meshParam->Joints.size();

		// setup joint name
		int len = (int)meshParam->ListJointName.size() + 1;

		wchar_t* listJointName = new wchar_t[len + 1];
		CStringImp::copy<wchar_t, const wchar_t>(listJointName, meshParam->ListJointName.c_str());

		wchar_t* p = listJointName;
		wchar_t* begin = listJointName;

		// set up joint
		for (int i = 0; i < nJoint; i++)
		{
			mesh->Joints.push_back(CSkinnedMesh::SJoint());
			CSkinnedMesh::SJoint& newJoint = mesh->Joints.getLast();

			SJointParam& joint = meshParam->Joints[i];

			std::wstring sidName;

			CJointData* jointData = NULL;
			bool end = false;

			do
			{
				// todo
				// we need split join name
				while (*p && !(*p == L' ' || *p == L'\n' || *p == L'\r' || *p == L'\t'))
					++p;

				*p = 0;

				if (*begin)
					sidName += std::wstring(begin);
				else
				{
					sidName += std::wstring(L"");
					end = true;
				}

				p++;

				// fix next new line
				while (*p && *p == L'\n')
					p++;
				begin = p;

				CStringImp::copy(name, sidName.c_str());
				jointData = findJointData(name);

				// we continue search joint name
				if (jointData == NULL)
					sidName += L" ";

			} while (jointData == NULL || end == true);

			// set joint name
			joint.Name = sidName;

			if (jointData != NULL)
			{
				newJoint.Name = jointData->BoneName;
				newJoint.EntityIndex = jointData->EntityIndex;
				newJoint.BindPoseMatrix.setbyproduct_nocheck(joint.InvMatrix, meshParam->BindShapeMatrix);
			}

			if (end == true)
			{
				mesh->Joints.clear();
				break;
			}
		}

		// free data
		delete[]listJointName;

		// setup vertex weight
		int numVertex = 0;
		for (u32 i = 0; i < meshParam->Triangles.size(); i++)
			numVertex += meshParam->Triangles[i].NumPolygon;

		// bone count per vertex
		numVertex = numVertex * 3;
		int* nBoneCount = new int[numVertex];
		memset(nBoneCount, 0, sizeof(int) * numVertex);

		// apply joint to vertex
		for (u32 i = 0, n = (u32)meshParam->JointIndex.size(); i < n; i += 2)
		{
			int boneID = meshParam->JointIndex[i];
			int weightID = meshParam->JointIndex[i + 1];

			SJointParam& joint = meshParam->Joints[boneID];
			SWeightParam& weight = joint.Weights[weightID];

			int nBone = nBoneCount[weight.VertexID]++;

			for (u32 i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
			{
				IMeshBuffer* buffer = (IMeshBuffer*)mesh->getMeshBuffer(i);

				video::S3DVertexSkin* vertex = NULL;
				video::S3DVertexSkinTangents* vertexTangent = NULL;

				if (m_createTangent == true)
					vertexTangent = (video::S3DVertexSkinTangents*)buffer->getVertexBuffer()->getVertices();
				else
					vertex = (video::S3DVertexSkin*)buffer->getVertexBuffer()->getVertices();


				// Add support FBX to DAE converter
				SColladaMeshVertexMap map;
				map.meshId = meshParam;
				map.bufferId = i;
				map.vertexId = weight.VertexID;

				std::vector<s32>& arrayVertexId = m_meshVertexIndex[map];
				u32 numVertexAffect = (u32)arrayVertexId.size();

				for (u32 j = 0; j < numVertexAffect; j++)
				{
					// todo find realVertexIndex
					s32 realVertexID = arrayVertexId[j];

					float* boneIndex = NULL;
					float* boneWeight = NULL;

					if (m_createTangent == true)
					{
						boneIndex = (float*)&(vertexTangent[realVertexID].BoneIndex);
						boneWeight = (float*)&(vertexTangent[realVertexID].BoneWeight);
					}
					else
					{
						boneIndex = (float*)&(vertex[realVertexID].BoneIndex);
						boneWeight = (float*)&(vertex[realVertexID].BoneWeight);
					}

					// only support 4 bones affect on 1 vertex
					// we need replace the smallest weight
					if (nBone >= 4)
					{
						int minIndex = 0;
						float minWeight = boneWeight[0];

						for (int i = 1; i < 4; i++)
						{
							if (boneWeight[i] < minWeight)
							{
								minIndex = i;
								minWeight = boneWeight[i];
								break;
							}
						}

						if (weight.Strength > minWeight)
						{
							boneIndex[minIndex] = (float)boneID;
							boneWeight[minIndex] = weight.Strength;
						}
					}
					else
					{
						boneIndex[nBone] = (float)boneID;
						boneWeight[nBone] = weight.Strength;
					}
				}

			}
		}

		delete[]nBoneCount;

		// fix the weight if vertex affect > 4 bone
		for (u32 i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* buffer = (IMeshBuffer*)mesh->getMeshBuffer(i);

			video::S3DVertexSkin* vertex = NULL;
			video::S3DVertexSkinTangents* vertexTangent = NULL;

			if (m_createTangent == true)
				vertexTangent = (video::S3DVertexSkinTangents*)buffer->getVertexBuffer()->getVertices();
			else
				vertex = (video::S3DVertexSkin*)buffer->getVertexBuffer()->getVertices();


			int numVertex = buffer->getVertexBuffer()->getVertexCount();

			for (int j = 0; j < numVertex; j++)
			{
				float* boneIndex = NULL;
				float* boneWeight = NULL;

				if (m_createTangent == true)
				{
					boneIndex = (float*)&(vertexTangent[j].BoneIndex);
					boneWeight = (float*)&(vertexTangent[j].BoneWeight);
				}
				else
				{
					boneIndex = (float*)&(vertex[j].BoneIndex);
					boneWeight = (float*)&(vertex[j].BoneWeight);
				}

				if ((int)boneIndex[3] != 0)
				{
					float totalWeight =
						boneWeight[0] +
						boneWeight[1] +
						boneWeight[2] +
						boneWeight[3];

					boneWeight[0] = boneWeight[0] / totalWeight;
					boneWeight[1] = boneWeight[1] / totalWeight;
					boneWeight[2] = boneWeight[2] / totalWeight;
					boneWeight[3] = boneWeight[3] / totalWeight;
				}
			}
		}

		mesh->setHardwareMappingHint(EHM_STATIC);
	}

	void CColladaLoader::cleanData()
	{
		m_listEffects.clear();
		m_listImages.clear();
		m_listMaterial.clear();

		m_nameToJointData.clear();
		m_sidToJointData.clear();

		ArrayMeshParams::iterator i = m_listMesh.begin(), end = m_listMesh.end();
		while (i != end)
		{
			SMeshParam& mesh = (*i);

			int n = (int)mesh.Buffers.size();
			int j = 0;

			for (j = 0; j < n; j++)
			{
				if (mesh.Buffers[j].FloatArray != NULL)
					delete mesh.Buffers[j].FloatArray;
			}

			n = (int)mesh.Triangles.size();
			for (j = 0; j < n; j++)
			{
				if (mesh.Triangles[j].VCount != NULL)
					delete mesh.Triangles[j].VCount;

				if (mesh.Triangles[j].IndexBuffer != NULL)
					delete mesh.Triangles[j].IndexBuffer;
			}

			i++;
		}
		m_listMesh.clear();

		for (u32 j = 0; j < m_listNode.size(); j++)
		{
			SNodeParam* pNode = m_listNode[j];

			std::stack<SNodeParam*>	stackNode;
			stackNode.push(pNode);
			while (stackNode.size())
			{
				pNode = stackNode.top();
				stackNode.pop();

				for (u32 i = 0; i < pNode->Childs.size(); i++)
					stackNode.push(pNode->Childs[i]);

				delete pNode;
			}
		}
		m_listNode.clear();
	}

}	// namespace
