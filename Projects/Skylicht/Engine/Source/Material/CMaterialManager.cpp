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

namespace Skylicht
{
	CMaterialManager::CMaterialManager()
	{

	}

	CMaterialManager::~CMaterialManager()
	{

	}

	void CMaterialManager::exportMaterial(CEntityPrefab *prefab, const char *folder, const char *filename)
	{
		std::string matFile = folder;
		matFile += "/";
		matFile += filename;

		IrrlichtDevice	*device = getIrrlichtDevice();
		io::IFileSystem *fs = device->getFileSystem();

		io::IWriteFile *writeFile = fs->createAndWriteFile(matFile.c_str());
		if (writeFile == NULL)
			return;

		std::string buffer;
		std::map<std::string, bool> saved;

		char data[1024];

		buffer += "<Materials>\n";

		CEntity** entities = prefab->getEntities();
		for (int i = 0, n = prefab->getNumEntities(); i < n; i++)
		{
			CRenderMeshData *renderer = entities[i]->getData<CRenderMeshData>();
			if (renderer != NULL)
			{
				CMesh *mesh = renderer->getMesh();
				if (mesh != NULL)
				{
					for (int j = 0, m = (int)mesh->getMeshBufferCount(); j < m; j++)
					{
						ITexture* texture = NULL;
						IMeshBuffer* meshBuffer = mesh->getMeshBuffer(j);

						if (meshBuffer != NULL && j < (int)mesh->MaterialName.size())
						{
							const char *materialName = mesh->MaterialName[j].c_str();
							if (saved[materialName] == false)
							{
								SMaterial& material = meshBuffer->getMaterial();

								CShader *shader = CShaderManager::getInstance()->getShaderByID(material.MaterialType);
								if (shader != NULL)
								{
									sprintf(data, "\t<Material name='%s' shader='%s'>\n", materialName, shader->getShaderPath().c_str());
									buffer += data;

									// write texture slot
									buffer += "\t\t<Textures>\n";
									for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
									{
										ITexture *texture = material.TextureLayer[i].Texture;
										if (texture != NULL)
										{
											sprintf(data, "\t\t\t<Texture slot='%d' path='%s'/>\n", i, texture->getName().getPath().c_str());
											buffer += data;
										}
									}
									buffer += "\t\t</Textures>\n";

									// write default shader params
									buffer += "\t\t<Params>\n";
									for (int i = 0, n = shader->getNumUI(); i < n; i++)
									{
										CShader::SUniformUI *uniformUI = shader->getUniformUI(i);
										CShader::SUniform* info = uniformUI->UniformInfo;
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
		writeFile->write(buffer.c_str(), buffer.size());
		writeFile->drop();
	}
}