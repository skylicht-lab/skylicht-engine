#include "pch.h"
#include "Utils/CPath.h"
#include "CMeshManager.h"

#include "Importer/Collada/CColladaLoader.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/CShader.h"

namespace Skylicht
{
	CMeshManager::CMeshManager()
	{

	}

	CMeshManager::~CMeshManager()
	{
		releaseAllPrefabs();
	}

	void CMeshManager::releaseAllPrefabs()
	{
		std::map<std::string, CEntityPrefab*>::iterator i = m_meshPrefabs.begin(), end = m_meshPrefabs.end();
		while (i != end)
		{
			delete (*i).second;
			++i;
		}

		m_meshPrefabs.clear();
	}

	CEntityPrefab* CMeshManager::loadModel(const char *resource, const char *texturePath, bool loadNormalMap, bool loadTexcoord2, bool createBatching)
	{
		// find in cached
		std::map<std::string, CEntityPrefab*>::iterator findCache = m_meshPrefabs.find(resource);
		if (findCache != m_meshPrefabs.end())
		{
			return (*findCache).second;
		}

		IMeshImporter *importer = NULL;
		CEntityPrefab *output = NULL;

		// load from file
		std::string ext = CPath::getFileNameExt(resource);
		if (ext == "dae")
		{
			importer = new CColladaLoader();
		}

		if (importer != NULL)
		{
			output = new CEntityPrefab();

			// add search texture path
			importer->addTextureFolder(texturePath);

			// load model
			if (importer->loadModel(resource, output, loadNormalMap, loadTexcoord2, createBatching) == true)
			{
				// cached resource
				m_meshPrefabs[resource] = output;
			}
			else
			{
				// load failed!
				delete output;
				output = NULL;
			}

			// clear cache load model
			delete importer;
		}

		return output;
	}

	void CMeshManager::exportMaterial(CEntityPrefab *prefab, const char *folder, const char *filename)
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