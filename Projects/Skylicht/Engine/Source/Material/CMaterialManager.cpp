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
#include "CMaterialManager.h"
#include "Shader/CShader.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CMaterialManager);

	CMaterialManager::CMaterialManager()
	{

	}

	CMaterialManager::~CMaterialManager()
	{
		releaseAllMaterials();
	}

	void CMaterialManager::releaseAllMaterials()
	{
		std::map<std::string, ArrayMaterial>::iterator i = m_materials.begin(), end = m_materials.end();
		while (i != end)
		{
			ArrayMaterial& list = (*i).second;
			for (int j = 0, n = (int)list.size(); j < n; j++)
			{
				CMaterial* m = list[j];
				m->drop();
			}
			list.clear();
			++i;
		}
		m_materials.clear();

		for (CMaterial* m : m_listGenerateMaterials)
			m->drop();
		m_listGenerateMaterials.clear();
	}

	ArrayMaterial& CMaterialManager::loadMaterial(const char* filename, bool loadTexture, const std::vector<std::string>& textureFolders)
	{
		// find in cached
		std::map<std::string, ArrayMaterial>::iterator findCache = m_materials.find(filename);
		if (findCache != m_materials.end())
		{
			return (*findCache).second;
		}

		// auto add base folder
		std::string baseFolder = CPath::getFolderPath(filename);

		std::vector<std::string> folders = textureFolders;
		folders.push_back(baseFolder);

		ArrayMaterial& result = m_materials[filename];

		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IXMLReader* xmlRead = fs->createXMLReader(filename);
		if (xmlRead == NULL)
			return result;

		const wchar_t* textw;
		char text[1024];

		CMaterial* material = NULL;

		CMaterial::SExtraParams* extra = NULL;

		while (xmlRead->read())
		{
			switch (xmlRead->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlRead->getNodeName();
				if (nodeName == L"Material")
				{
					textw = xmlRead->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(textw, text);
					std::string name = text;

					textw = xmlRead->getAttributeValue(L"shader");
					CStringImp::convertUnicodeToUTF8(textw, text);
					std::string shader = text;

					// create material
					material = new CMaterial(name.c_str(), shader.c_str());
					material->setMaterialPath(filename);
					material->updateShaderParams();

					extra = NULL;

					if (loadTexture == true)
						material->loadDefaultTexture();

					result.push_back(material);
				}
				if (nodeName == L"Extra")
				{
					textw = xmlRead->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(textw, text);
					extra = material->newExtra(text);
				}
				else if (nodeName == L"Property")
				{
					if (material != NULL)
					{
						textw = xmlRead->getAttributeValue(L"name");
						CStringImp::convertUnicodeToUTF8(textw, text);
						std::string name = text;

						textw = xmlRead->getAttributeValue(L"value");
						CStringImp::convertUnicodeToUTF8(textw, text);
						std::string value = text;

						// set property (depth, culling)
						material->setProperty(name, value);
					}
				}
				else if (nodeName == L"Texture")
				{
					if (material != NULL)
					{
						textw = xmlRead->getAttributeValue(L"path");
						CStringImp::convertUnicodeToUTF8(textw, text);
						std::string path = text;

						// load texture by name
						textw = xmlRead->getAttributeValue(L"name");
						std::string name;
						if (textw != NULL)
						{
							CStringImp::convertUnicodeToUTF8(textw, text);
							if (path.empty() == false)
							{
								name = text;
								if (extra == NULL)
									material->setUniformTexture(name.c_str(), path.c_str(), folders, loadTexture);
								else
									material->setExtraUniformTexture(extra, name.c_str(), path.c_str());
							}
						}

						// wrapU
						textw = xmlRead->getAttributeValue(L"wrapU");
						if (textw != NULL)
						{
							CStringImp::convertUnicodeToUTF8(textw, text);
							int wrapU = atoi(text);
							if (extra == NULL)
								material->setUniformTextureWrapU(name.c_str(), wrapU);
							else
								material->setExtraUniformTextureWrapU(extra, name.c_str(), wrapU);
						}

						// wrapV
						textw = xmlRead->getAttributeValue(L"wrapV");
						if (textw != NULL)
						{
							CStringImp::convertUnicodeToUTF8(textw, text);
							int wrapV = atoi(text);
							if (extra == NULL)
								material->setUniformTextureWrapV(name.c_str(), wrapV);
							else
								material->setExtraUniformTextureWrapV(extra, name.c_str(), wrapV);
						}
					}
				}
				else if (nodeName == L"Param")
				{
					if (material != NULL)
					{
						textw = xmlRead->getAttributeValue(L"name");
						CStringImp::convertUnicodeToUTF8(textw, text);
						std::string name = text;

						textw = xmlRead->getAttributeValue(L"floatSize");
						CStringImp::convertUnicodeToUTF8(textw, text);
						int floatSize = atoi(text);

						textw = xmlRead->getAttributeValue(L"floatValue");
						CStringImp::convertUnicodeToUTF8(textw, text);
						float v[4];
						sscanf(text, "%f,%f,%f,%f", &v[0], &v[1], &v[2], &v[3]);

						// set uniform param
						if (extra == NULL)
						{
							if (floatSize == 1)
								material->setUniform(name.c_str(), v[0]);
							else if (floatSize == 2)
								material->setUniform2(name.c_str(), v);
							else if (floatSize == 3)
								material->setUniform3(name.c_str(), v);
							else if (floatSize == 4)
								material->setUniform4(name.c_str(), v);
						}
						else
						{
							material->setExtraUniform(extra, name.c_str(), v, floatSize);
						}
					}
				}
			}
			case io::EXN_ELEMENT_END:
			{
			}
			break;
			case io::EXN_TEXT:
			{
			}
			break;
			default:
				break;					
			}
		}

		xmlRead->drop();

		// need update shader params
		for (CMaterial* m : result)
			m->updateShaderParams();

		return result;
	}

	CMaterial* CMaterialManager::createMaterial(ArrayMaterial& materials)
	{
		CMaterial* material = new CMaterial("NewMaterial", "BuiltIn/Shader/Basic/TextureColor.xml");
		materials.push_back(material);

		if (materials.size() >= 2)
		{
			const char* path = materials[0]->getMaterialPath();
			material->setMaterialPath(path);
			m_materials[path] = materials;
		}

		return material;
	}

	void CMaterialManager::deleteMaterial(ArrayMaterial& materials, CMaterial* material)
	{
		std::string cachePath = material->getMaterialPath();

		// delete on the list
		ArrayMaterial::iterator i = materials.begin(), e = materials.end();
		while (i != e)
		{
			if ((*i) == material)
			{
				materials.erase(i);
				break;
			}
			++i;
		}

		// also delete on the cache
		ArrayMaterial& m = m_materials[cachePath];
		i = m.begin();
		e = m.end();
		while (i != e)
		{
			if ((*i) == material)
			{
				m.erase(i);
				material->drop();
				break;
			}
			++i;
		}
	}

	void CMaterialManager::saveMaterial(const ArrayMaterial& materials, const char* filename)
	{
		// save data
		std::string matFile = filename;

		char tempPath[512];
		char relativeTextureFolder[512];
		CStringImp::replaceText<char>(tempPath, filename, "\\", "/");
		std::string materialFolder = CPath::getFolderPath(tempPath);

		// fix if run from Bin folder (SampleApplication)
		CStringImp::replaceText(relativeTextureFolder, materialFolder.c_str(), "../Assets/", "");

		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IWriteFile* writeFile = fs->createAndWriteFile(matFile.c_str());
		if (writeFile == NULL)
			return;

		std::string buffer;
		std::map<std::string, bool> saved;

		char data[1024];

		buffer += "<!-- File generated by function CMaterialManager::exportMaterial -->\n";
		buffer += "<Materials>\n";

		std::string cachePath;

		for (CMaterial* material : materials)
		{
			cachePath = material->getMaterialPath();

			sprintf(data, "\t<Material name='%s' shader='%s'>\n", material->getName(), material->getShaderPath());
			buffer += data;

			std::vector<CMaterial::SUniformTexture*>& textures = material->getUniformTexture();
			std::vector<CMaterial::SUniformValue*>& params = material->getUniformParams();
			std::vector<CMaterial::SExtraParams*>& extras = material->getExtraParams();

			if (textures.size() > 0)
			{
				// write uniform texture
				buffer += "\t\t<Textures>\n";
				for (CMaterial::SUniformTexture* texture : textures)
				{
					sprintf(data, "\t\t\t<Texture name='%s' path='%s' wrapU='%d' wrapV='%d'/>\n",
						texture->Name.c_str(),
						CPath::getRelativePath(texture->Path, relativeTextureFolder).c_str(),
						texture->WrapU,
						texture->WrapV);
					buffer += data;
				}
				buffer += "\t\t</Textures>\n";
			}

			if (params.size() > 0)
			{
				// write uniform value
				buffer += "\t\t<Params>\n";
				for (CMaterial::SUniformValue* param : params)
				{
					sprintf(data, "\t\t\t<Param name='%s' floatSize='%d' floatValue='%f,%f,%f,%f'/>\n",
						param->Name.c_str(),
						param->FloatSize,
						param->FloatValue[0],
						param->FloatValue[1],
						param->FloatValue[2],
						param->FloatValue[3]);
					buffer += data;
				}
				buffer += "\t\t</Params>\n";
			}

			buffer += "\t\t<Properties>\n";
			const char* propertyName[] = {
				"ZBuffer",
				"ZWriteEnable",
				"BackfaceCulling",
				"FrontfaceCulling",
				"DoubleSided"
			};
			for (int i = 0; i < 4; i++)
			{
				sprintf(data, "\t\t\t<Property name='%s' value='%s'/>\n",
					propertyName[i],
					material->getProperty(propertyName[i]).c_str());
				buffer += data;
			}
			buffer += "\t\t</Properties>\n";

			// write extra data
			if (extras.size() > 0)
			{
				buffer += "\t\t<Extras>\n";
				for (CMaterial::SExtraParams* e : extras)
				{
					sprintf(data, "\t\t\t<Extra name='%s'/>\n", e->ShaderPath.c_str());
					buffer += data;

					if (e->UniformTextures.size() > 0)
					{
						// write uniform texture
						buffer += "\t\t\t\t<Textures>\n";
						for (CMaterial::SUniformTexture* texture : e->UniformTextures)
						{
							sprintf(data, "\t\t\t\t\t<Texture name='%s' path='%s' wrapU='%d' wrapV='%d'/>\n",
								texture->Name.c_str(),
								CPath::getRelativePath(texture->Path, relativeTextureFolder).c_str(),
								texture->WrapU,
								texture->WrapV);
							buffer += data;
						}
						buffer += "\t\t\t\t</Textures>\n";
					}

					if (e->UniformParams.size() > 0)
					{
						// write uniform value
						buffer += "\t\t\t\t<Params>\n";
						for (CMaterial::SUniformValue* param : e->UniformParams)
						{
							sprintf(data, "\t\t\t\t\t<Param name='%s' floatSize='%d' floatValue='%f,%f,%f,%f'/>\n",
								param->Name.c_str(),
								param->FloatSize,
								param->FloatValue[0],
								param->FloatValue[1],
								param->FloatValue[2],
								param->FloatValue[3]);
							buffer += data;
						}
						buffer += "\t\t\t\t</Params>\n";
					}

					buffer += "\t\t\t</Extra>\n";
				}
				buffer += "\t\t</Extras>\n";
			}

			buffer += "\t</Material>\n";
		}

		buffer += "</Materials>";

		CStringImp::replaceString(buffer, std::string("\t"), std::string("    "));

		writeFile->write(buffer.c_str(), (u32)buffer.size());
		writeFile->drop();

		// save the cache
		if (!cachePath.empty())
		{
			m_materials[cachePath] = materials;
		}
	}

	void CMaterialManager::exportMaterial(CEntityPrefab* prefab, const char* filename)
	{
		std::string matFile = filename;

		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		char tempPath[512];
		char relativeTextureFolder[512];
		CStringImp::replaceText<char>(tempPath, filename, "\\", "/");
		std::string materialFolder = CPath::getFolderPath(tempPath);
		CStringImp::replaceText(relativeTextureFolder, materialFolder.c_str(), "../Assets/", "");

		io::IWriteFile* writeFile = fs->createAndWriteFile(matFile.c_str());
		if (writeFile == NULL)
			return;

		std::string buffer;
		std::map<std::string, bool> saved;

		char data[1024];

		buffer += "<!-- File generated by function CMaterialManager::exportMaterial -->\n";
		buffer += "<Materials>\n";

		CEntity** entities = prefab->getEntities();
		for (int i = 0, n = prefab->getNumEntities(); i < n; i++)
		{
			CRenderMeshData* renderer = GET_ENTITY_DATA(entities[i], CRenderMeshData);
			if (renderer != NULL)
			{
				CMesh* mesh = renderer->getMesh();
				if (mesh != NULL)
				{
					for (int j = 0, m = (int)mesh->getMeshBufferCount(); j < m; j++)
					{
						ITexture* texture = NULL;
						IMeshBuffer* meshBuffer = mesh->getMeshBuffer(j);

						if (meshBuffer != NULL && j < (int)mesh->MaterialName.size())
						{
							const char* materialName = mesh->MaterialName[j].c_str();
							if (saved[materialName] == false)
							{
								SMaterial& material = meshBuffer->getMaterial();

								CShader* shader = CShaderManager::getInstance()->getShaderByID(material.MaterialType);
								if (shader != NULL)
								{
									sprintf(data, "\t<Material name='%s' shader='%s'>\n", materialName, shader->getShaderPath().c_str());
									buffer += data;

									// write texture slot
									buffer += "\t\t<Textures>\n";
									for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
									{
										ITexture* texture = material.TextureLayer[i].Texture;
										if (texture != NULL)
										{
											std::string texturePath = texture->getName().getPath().c_str();
											texturePath = CPath::replaceFileExt(texturePath, ".tga");

											int pos = CStringImp::find(texturePath.c_str(), "Assets");
											if (pos > 0)
											{
												char tempPath[1024];
												CStringImp::mid(tempPath, texturePath.c_str(), pos + 7, (int)texturePath.length());
												texturePath = tempPath;
											}

											sprintf(data, "\t\t\t<Texture slot='%d' path='%s'/>\n", i,
												CPath::getRelativePath(texturePath, relativeTextureFolder).c_str());
											buffer += data;
										}
									}
									buffer += "\t\t</Textures>\n";

									// write default shader params
									buffer += "\t\t<Params>\n";
									for (int i = 0, n = shader->getNumUI(); i < n; i++)
									{
										CShader::SUniformUI* uniformUI = shader->getUniformUI(i);
										SUniform* info = uniformUI->UniformInfo;
										if (info != NULL)
										{
											sprintf(data, "\t\t\t<Param name='%s' floatSize='%d' floatValue='%f,%f,%f,%f'/>\n",
												info->Name.c_str(),
												info->FloatSize,
												info->Value[0],
												info->Value[1],
												info->Value[2],
												info->Value[3]);
											buffer += data;
										}
									}
									buffer += "\t\t</Params>\n";

									buffer += "\t</Material>\n";
									saved[materialName] = true;
								}
							}
						}
					}
				}
			}
		}

		buffer += "</Materials>";

		CStringImp::replaceString(buffer, std::string("\t"), std::string("    "));

		writeFile->write(buffer.c_str(), (u32)buffer.size());
		writeFile->drop();
	}

	ArrayMaterial CMaterialManager::initDefaultMaterial(CEntityPrefab* prefab)
	{
		CMaterial* materialObj;
		ArrayMaterial result;

		std::map<std::string, bool> saved;

		CEntity** entities = prefab->getEntities();
		for (int i = 0, n = prefab->getNumEntities(); i < n; i++)
		{
			CRenderMeshData* renderer = GET_ENTITY_DATA(entities[i], CRenderMeshData);
			if (renderer != NULL)
			{
				CMesh* mesh = renderer->getMesh();
				if (mesh != NULL)
				{
					for (int j = 0, m = (int)mesh->getMeshBufferCount(); j < m; j++)
					{
						ITexture* texture = NULL;
						IMeshBuffer* meshBuffer = mesh->getMeshBuffer(j);

						if (meshBuffer != NULL && j < (int)mesh->MaterialName.size())
						{
							const char* materialName = mesh->MaterialName[j].c_str();
							if (saved[materialName] == false)
							{
								SMaterial& material = meshBuffer->getMaterial();

								CShader* shader = CShaderManager::getInstance()->getShaderByID(material.MaterialType);
								if (shader != NULL)
								{
									materialObj = new CMaterial(materialName, shader->getShaderPath().c_str());
									materialObj->loadDefaultTexture();

									ITexture* t[MATERIAL_MAX_TEXTURES];
									for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
										t[i] = material.TextureLayer[i].Texture;

									materialObj->setTexture(t, MATERIAL_MAX_TEXTURES);

									for (int i = 0, n = shader->getNumUI(); i < n; i++)
									{
										CShader::SUniformUI* uniformUI = shader->getUniformUI(i);
										SUniform* info = uniformUI->UniformInfo;
										if (info != NULL)
										{
											if (info->FloatSize == 1)
												materialObj->setUniform(info->Name.c_str(), info->Value[0]);
											else if (info->FloatSize == 2)
												materialObj->setUniform2(info->Name.c_str(), info->Value);
											else if (info->FloatSize == 3)
												materialObj->setUniform3(info->Name.c_str(), info->Value);
											else if (info->FloatSize == 4)
												materialObj->setUniform4(info->Name.c_str(), info->Value);
										}
									}


									saved[materialName] = true;
									result.push_back(materialObj);

									m_listGenerateMaterials.push_back(materialObj);
								}
								else
								{
									char log[512];
									sprintf(log, "[CMaterialManager] initDefaultMaterial, %s missing shader %d", materialName, material.MaterialType);
									os::Printer::log(log);
								}
							}
						}
					}
				}
			}
		}

		return result;
	}
}
