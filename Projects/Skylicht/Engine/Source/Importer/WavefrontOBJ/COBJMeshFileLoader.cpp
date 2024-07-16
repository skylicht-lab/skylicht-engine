#include "pch.h"
#include "COBJMeshFileLoader.h"
#include "TextureManager/CTextureManager.h"

#include "Entity/CEntityPrefab.h"
#include "Transform/CWorldTransformData.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Culling/CCullingData.h"

#include "Importer/Utils/CMeshUtils.h"
#include "Material/Shader/CShaderManager.h"

#include "Utils/CPath.h"

#define WORD_BUFFER_LENGTH 512

namespace Skylicht
{
	COBJMeshFileLoader::COBJMeshFileLoader() :
		m_flipNormalMap(true)
	{

	}

	COBJMeshFileLoader::~COBJMeshFileLoader()
	{
		cleanUp();
	}

	void COBJMeshFileLoader::cleanUp()
	{
		for (u32 i = 0; i < m_materials.size(); ++i)
		{
			m_materials[i]->Meshbuffer->drop();
			delete m_materials[i];
		}

		m_materials.clear();
	}

	bool COBJMeshFileLoader::loadModel(const char* resource, CEntityPrefab* output, bool normalMap, bool flipNormalMap, bool texcoord2, bool batching)
	{
		IrrlichtDevice* device = getIrrlichtDevice();
		io::IFileSystem* fs = device->getFileSystem();

		io::IReadFile* file = fs->createAndOpenFile(resource);
		if (file == NULL)
		{
			char log[64];
			sprintf(log, "Can not load: %s\n", resource);
			os::Printer::log(log);
			return false;
		}

		const long filesize = file->getSize();
		if (!filesize)
			return 0;

		c8* buf = new c8[filesize];
		memset(buf, 0, filesize);
		file->read((void*)buf, (u32)filesize);
		const c8* const bufEnd = buf + filesize;

		// Process obj information
		const c8* bufPtr = buf;
		core::stringc grpName, mtlName;

		bool mtlChanged = false;
		bool useGroups = true;
		bool useMaterials = true;

		core::array<core::vector3df> vertexBuffer;
		core::array<core::vector3df> normalsBuffer;
		core::array<core::vector2df> textureCoordBuffer;

		const io::path relPath = fs->getFileDir(resource) + "/";
		const io::path fullName = file->getFileName();

		m_modelName = fullName.c_str();

		SObjMtl* currMtl = new SObjMtl();
		m_materials.push_back(currMtl);

		// set default name is model name
		currMtl->Name = CPath::getFileName(m_modelName).c_str();

		while (bufPtr != bufEnd)
		{
			switch (bufPtr[0])
			{
			case 'm':	// mtllib (material)
			{
				if (useMaterials)
				{
					c8 name[WORD_BUFFER_LENGTH];
					bufPtr = goAndCopyNextWord(name, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
#ifdef _IRR_DEBUG_OBJ_LOADER_
					os::Printer::log("Reading material file", name);
#endif
					readMTL(name, relPath);
				}
			}
			break;

			case 'v':               // v, vn, vt
				switch (bufPtr[1])
				{
				case ' ':          // vertex
				{
					core::vector3df vec;
					bufPtr = readVec3(bufPtr, vec, bufEnd);
					vertexBuffer.push_back(vec);
				}
				break;

				case 'n':       // normal
				{
					core::vector3df vec;
					bufPtr = readVec3(bufPtr, vec, bufEnd);
					normalsBuffer.push_back(vec);
				}
				break;

				case 't':       // texcoord
				{
					core::vector2df vec;
					bufPtr = readUV(bufPtr, vec, bufEnd);
					textureCoordBuffer.push_back(vec);
				}
				break;
				}
				break;

			case 'g': // group name
			{
				c8 grp[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(grp, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
#ifdef _IRR_DEBUG_OBJ_LOADER_
				os::Printer::log("Loaded group start", grp, ELL_DEBUG);
#endif
				if (useGroups)
				{
					if (0 != grp[0])
						grpName = grp;
					else
						grpName = "default";
				}
				mtlChanged = true;
			}
			break;

			case 's': // smoothing can be a group or off (equiv. to 0)
			{
				c8 smooth[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(smooth, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
#ifdef _IRR_DEBUG_OBJ_LOADER_
				os::Printer::log("Loaded smoothing group start", smooth, ELL_DEBUG);
#endif
				//if (core::stringc("off") == smooth)
				//	smoothingGroup = 0;
				//else
				//	smoothingGroup = core::strtoul10(smooth);
			}
			break;

			case 'u': // usemtl
				// get name of material
			{
				c8 matName[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(matName, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
#ifdef _IRR_DEBUG_OBJ_LOADER_
				os::Printer::log("Loaded material start", matName, ELL_DEBUG);
#endif
				mtlName = matName;
				mtlChanged = true;
			}
			break;

			case 'f':               // face
			{
				c8 vertexWord[WORD_BUFFER_LENGTH]; // for retrieving vertex data
				video::S3DVertex v;
				// Assign vertex color from currently active material's diffuse color
				if (mtlChanged)
				{
					// retrieve the material
					SObjMtl* useMtl = findMtl(mtlName, grpName);
					// only change material if we found it
					if (useMtl)
						currMtl = useMtl;
					mtlChanged = false;
				}
				if (currMtl)
					v.Color = currMtl->Meshbuffer->getMaterial().DiffuseColor;

				// get all vertices data in this face (current line of obj file)
				const core::stringc wordBuffer = copyLine(bufPtr, bufEnd);
				const c8* linePtr = wordBuffer.c_str();
				const c8* const endPtr = linePtr + wordBuffer.size();

				core::array<int> faceCorners;
				faceCorners.reallocate(32); // should be large enough

				// read in all vertices
				linePtr = goNextWord(linePtr, endPtr);
				while (0 != linePtr[0])
				{
					// Array to communicate with retrieveVertexIndices()
					// sends the buffer sizes and gets the actual indices
					// if index not set returns -1
					s32 Idx[3];
					Idx[0] = Idx[1] = Idx[2] = -1;

					// read in next vertex's data
					u32 wlength = copyWord(vertexWord, linePtr, WORD_BUFFER_LENGTH, endPtr);
					// this function will also convert obj's 1-based index to c++'s 0-based index
					retrieveVertexIndices(vertexWord, Idx, vertexWord + wlength + 1, vertexBuffer.size(), textureCoordBuffer.size(), normalsBuffer.size());
					if (-1 != Idx[0] && Idx[0] < (irr::s32)vertexBuffer.size())
						v.Pos = vertexBuffer[Idx[0]];
					else
					{
						os::Printer::log("Invalid vertex index in this line:", wordBuffer.c_str(), ELL_ERROR);
						delete[] buf;
						return 0;
					}
					if (-1 != Idx[1] && Idx[1] < (irr::s32)textureCoordBuffer.size())
						v.TCoords = textureCoordBuffer[Idx[1]];
					else
						v.TCoords.set(0.0f, 0.0f);
					if (-1 != Idx[2] && Idx[2] < (irr::s32)normalsBuffer.size())
						v.Normal = normalsBuffer[Idx[2]];
					else
					{
						v.Normal.set(0.0f, 0.0f, 0.0f);
						currMtl->RecalculateNormals = true;
					}

					int vertLocation;
					core::map<video::S3DVertex, u32>::Node* n = currMtl->VertMap.find(v);
					if (n)
					{
						vertLocation = n->getValue();
					}
					else
					{
						currMtl->Meshbuffer->getVertexBuffer()->addVertex((void*)&v);
						vertLocation = currMtl->Meshbuffer->getVertexBuffer()->getVertexCount() - 1;
						currMtl->VertMap.insert(v, vertLocation);
					}

					faceCorners.push_back(vertLocation);

					// go to next vertex
					linePtr = goNextWord(linePtr, endPtr);
				}

				// triangulate the face
				for (u32 i = 1; i < faceCorners.size() - 1; ++i)
				{
					// Add a triangle
					currMtl->Meshbuffer->getIndexBuffer()->addIndex(faceCorners[i + 1]);
					currMtl->Meshbuffer->getIndexBuffer()->addIndex(faceCorners[i]);
					currMtl->Meshbuffer->getIndexBuffer()->addIndex(faceCorners[0]);
				}
				faceCorners.set_used(0); // fast clear
				faceCorners.reallocate(32);
			}
			break;

			case '#': // comment
			default:
				break;
			}	// end switch(bufPtr[0])
			// eat up rest of line
			bufPtr = goNextLine(bufPtr, bufEnd);
		}	// end while(bufPtr && (bufPtr-buf<filesize))

		constructScene(output, normalMap);

		file->drop();
		delete[] buf;
		return true;
	}

	void COBJMeshFileLoader::constructScene(CEntityPrefab* output, bool normalMap)
	{
		// create entity
		CEntity* root = output->createEntity();
		output->addTransformData(root, NULL, core::IdentityMatrix, m_modelName.c_str());

		IMeshManipulator* meshManipulator = getIrrlichtDevice()->getSceneManager()->getMeshManipulator();

		// Combine all the groups (meshbuffers) into the mesh
		for (u32 m = 0; m < m_materials.size(); ++m)
		{
			if (m_materials[m]->Meshbuffer->getIndexBuffer()->getIndexCount() > 0)
			{
				CMesh* mesh = new CMesh();

				// add entity
				CEntity* entity = output->createEntity();
				output->addTransformData(entity, root, core::IdentityMatrix, "");

				// add culling data
				entity->addData<CCullingData>();

				m_materials[m]->Meshbuffer->recalculateBoundingBox();
				if (m_materials[m]->RecalculateNormals)
					meshManipulator->recalculateNormals(m_materials[m]->Meshbuffer);

				// if this material have normal map
				if (m_materials[m]->Meshbuffer->getMaterial().TextureLayer[1].Texture != NULL || normalMap == true)
				{
					CMeshUtils::convertToTangentVertices(m_materials[m]->Meshbuffer, m_flipNormalMap);

					// add tangent mesh buffer
					mesh->addMeshBuffer(m_materials[m]->Meshbuffer, m_materials[m]->Name.c_str());
				}
				else
				{
					// add mesh buffer
					mesh->addMeshBuffer(m_materials[m]->Meshbuffer, m_materials[m]->Name.c_str());
				}

				mesh->recalculateBoundingBox();

				// notify create static hardware buffer
				mesh->setHardwareMappingHint(EHM_STATIC);

				// add & clone new render mesh
				CRenderMeshData* meshData = entity->addData<CRenderMeshData>();
				meshData->setMesh(mesh);

				// drop this mesh
				mesh->drop();
			}
		}
	}

	void COBJMeshFileLoader::readMTL(const c8* fileName, const io::path& relPath)
	{
		const io::path realFile(fileName);
		io::IReadFile* mtlReader;

		io::IFileSystem* fileSystem = getIrrlichtDevice()->getFileSystem();

		if (fileSystem->existFile(realFile))
			mtlReader = fileSystem->createAndOpenFile(realFile);
		else if (fileSystem->existFile(relPath + realFile))
			mtlReader = fileSystem->createAndOpenFile(relPath + realFile);
		else if (fileSystem->existFile(fileSystem->getFileBasename(realFile)))
			mtlReader = fileSystem->createAndOpenFile(fileSystem->getFileBasename(realFile));
		else
			mtlReader = fileSystem->createAndOpenFile(relPath + fileSystem->getFileBasename(realFile));

		if (!mtlReader)	// fail to open and read file
		{
			os::Printer::log("Could not open material file", realFile, ELL_WARNING);
			return;
		}

		const long filesize = mtlReader->getSize();
		if (!filesize)
		{
			os::Printer::log("Skipping empty material file", realFile, ELL_WARNING);
			mtlReader->drop();
			return;
		}

		c8* buf = new c8[filesize];
		mtlReader->read((void*)buf, (u32)filesize);
		const c8* bufEnd = buf + filesize;

		SObjMtl* currMaterial = 0;

		const c8* bufPtr = buf;
		while (bufPtr != bufEnd)
		{
			switch (*bufPtr)
			{
			case 'n': // newmtl
			{
				// if there's an existing material, store it first
				if (currMaterial)
					m_materials.push_back(currMaterial);

				// extract new material's name
				c8 mtlNameBuf[WORD_BUFFER_LENGTH];
				bufPtr = goAndCopyNextWord(mtlNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

				currMaterial = new SObjMtl();
				currMaterial->Name = mtlNameBuf;
			}
			break;
			case 'i': // illum - illumination
				if (currMaterial)
				{
					const u32 COLOR_BUFFER_LENGTH = 16;
					c8 illumStr[COLOR_BUFFER_LENGTH];

					bufPtr = goAndCopyNextWord(illumStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
					currMaterial->Illumination = (c8)atol(illumStr);
				}
				break;
			case 'N':
				if (currMaterial)
				{
					switch (bufPtr[1])
					{
					case 's': // Ns - shininess
					{
						const u32 COLOR_BUFFER_LENGTH = 16;
						c8 nsStr[COLOR_BUFFER_LENGTH];

						bufPtr = goAndCopyNextWord(nsStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
						f32 shininessValue = core::fast_atof(nsStr);

						// wavefront shininess is from [0, 1000], so scale for OpenGL
						shininessValue *= 0.128f;
						currMaterial->Meshbuffer->getMaterial().Shininess = shininessValue;
					}
					break;
					case 'i': // Ni - refraction index
					{
						c8 tmpbuf[WORD_BUFFER_LENGTH];
						bufPtr = goAndCopyNextWord(tmpbuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					}
					break;
					}
				}
				break;
			case 'K':
				if (currMaterial)
				{
					switch (bufPtr[1])
					{
					case 'd': // Kd = diffuse
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->getMaterial().DiffuseColor, bufEnd);
					}
					break;

					case 's': // Ks = specular
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->getMaterial().SpecularColor, bufEnd);
					}
					break;

					case 'a': // Ka = ambience
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->getMaterial().AmbientColor, bufEnd);
					}
					break;
					case 'e': // Ke = emissive
					{
						bufPtr = readColor(bufPtr, currMaterial->Meshbuffer->getMaterial().EmissiveColor, bufEnd);
					}
					break;
					}	// end switch(bufPtr[1])
				}	// end case 'K': if ( 0 != currMaterial )...
				break;
			case 'b': // bump
			case 'm': // texture maps
				if (currMaterial)
				{
					bufPtr = readTextures(bufPtr, bufEnd, currMaterial, relPath);
				}
				break;
			case 'd': // d - transparency
				if (currMaterial)
				{
					const u32 COLOR_BUFFER_LENGTH = 16;
					c8 dStr[COLOR_BUFFER_LENGTH];

					bufPtr = goAndCopyNextWord(dStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
					f32 dValue = core::fast_atof(dStr);

					currMaterial->Meshbuffer->getMaterial().DiffuseColor.setAlpha((s32)(dValue * 255));
					if (dValue < 1.0f)
						currMaterial->Meshbuffer->getMaterial().MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
				}
				break;
			case 'T':
				if (currMaterial)
				{
					switch (bufPtr[1])
					{
					case 'f':		// Tf - Transmitivity
						const u32 COLOR_BUFFER_LENGTH = 16;
						c8 redStr[COLOR_BUFFER_LENGTH];
						c8 greenStr[COLOR_BUFFER_LENGTH];
						c8 blueStr[COLOR_BUFFER_LENGTH];

						bufPtr = goAndCopyNextWord(redStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
						bufPtr = goAndCopyNextWord(greenStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
						bufPtr = goAndCopyNextWord(blueStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);

						f32 transparency = (core::fast_atof(redStr) + core::fast_atof(greenStr) + core::fast_atof(blueStr)) / 3;

						currMaterial->Meshbuffer->getMaterial().DiffuseColor.setAlpha((s32)(transparency * 255));
						if (transparency < 1.0f)
							currMaterial->Meshbuffer->getMaterial().MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
					}
				}
				break;
			default: // comments or not recognised
				break;
			} // end switch(bufPtr[0])
			// go to next line
			bufPtr = goNextLine(bufPtr, bufEnd);
		}	// end while (bufPtr)

		// end of file. if there's an existing material, store it
		if (currMaterial)
			m_materials.push_back(currMaterial);

		delete[] buf;
		mtlReader->drop();
	}

	SObjMtl* COBJMeshFileLoader::findMtl(const core::stringc& mtlName, const core::stringc& grpName)
	{
		SObjMtl* defMaterial = NULL;

		// search existing Materials for best match
		// exact match does return immediately, only name match means a new group
		for (u32 i = 0; i < m_materials.size(); ++i)
		{
			if (m_materials[i]->Name == mtlName)
			{
				if (m_materials[i]->Group == grpName)
					return m_materials[i];
				else
					defMaterial = m_materials[i];
			}
		}
		// we found a partial match
		if (defMaterial)
		{
			m_materials.push_back(new SObjMtl(*defMaterial));
			m_materials.getLast()->Group = grpName;
			return m_materials.getLast();
		}
		// we found a new group for a non-existant material
		else if (grpName.size())
		{
			m_materials.push_back(new SObjMtl(*m_materials[0]));
			m_materials.getLast()->Group = grpName;
			return m_materials.getLast();
		}
		return 0;
	}

	bool COBJMeshFileLoader::retrieveVertexIndices(c8* vertexData, s32* idx, const c8* bufEnd, u32 vbsize, u32 vtsize, u32 vnsize)
	{
		c8 word[16] = "";
		const c8* p = goFirstWord(vertexData, bufEnd);
		u32 idxType = 0;	// 0 = posIdx, 1 = texcoordIdx, 2 = normalIdx

		u32 i = 0;
		while (p != bufEnd)
		{
			if ((core::isdigit(*p)) || (*p == '-'))
			{
				// build up the number
				word[i++] = *p;
			}
			else if (*p == '/' || *p == ' ' || *p == '\0')
			{
				// number is completed. Convert and store it
				word[i] = '\0';
				// if no number was found index will become 0 and later on -1 by decrement
				idx[idxType] = core::strtol10(word);
				if (idx[idxType] < 0)
				{
					switch (idxType)
					{
					case 0:
						idx[idxType] += vbsize;
						break;
					case 1:
						idx[idxType] += vtsize;
						break;
					case 2:
						idx[idxType] += vnsize;
						break;
					}
				}
				else
					idx[idxType] -= 1;

				// reset the word
				word[0] = '\0';
				i = 0;

				// go to the next kind of index type
				if (*p == '/')
				{
					if (++idxType > 2)
					{
						// error checking, shouldn't reach here unless file is wrong
						idxType = 0;
					}
				}
				else
				{
					// set all missing values to disable (=-1)
					while (++idxType < 3)
						idx[idxType] = -1;
					++p;
					break; // while
				}
			}

			// go to the next char
			++p;
		}

		return true;
	}

	const c8* COBJMeshFileLoader::readTextures(const c8* bufPtr, const c8* const bufEnd, SObjMtl* currMaterial, const io::path& relPath)
	{
		u8 type = 0;

		// map_Kd - diffuse color texture map
		// map_Ks - specular color texture map
		// map_Ka - ambient color texture map
		// map_Ns - shininess texture map
		if (!strncmp(bufPtr, "map_Kd", 8))
			type = 0; // diffuse map
		else if ((!strncmp(bufPtr, "map_bump", 8)) || (!strncmp(bufPtr, "bump", 4)))
			type = 1; // normal map
		else if ((!strncmp(bufPtr, "map_d", 5)) || (!strncmp(bufPtr, "map_opacity", 11)))
			type = 2; // opacity map
		else if (!strncmp(bufPtr, "map_refl", 8))
			type = 3; // reflection map
		if (!strncmp(bufPtr, "map_Ks", 8))
			type = 4; // specular map

		// extract new material's name
		c8 textureNameBuf[WORD_BUFFER_LENGTH];
		bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

		f32 bumpiness = 6.0f;
		bool clamp = false;

		float materialParam = 0.0f;

		// handle options
		while (textureNameBuf[0] == '-')
		{
			if (!strncmp(bufPtr, "-bm", 3))
			{
				bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				// materialParam = core::fast_atof(textureNameBuf);
				bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				continue;
			}
			else
				if (!strncmp(bufPtr, "-blendu", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-blendv", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-cc", 3))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-clamp", 6))
					bufPtr = readBool(bufPtr, clamp, bufEnd);
				else if (!strncmp(bufPtr, "-texres", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-type", 5))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-mm", 3))
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				}
				else if (!strncmp(bufPtr, "-o", 2)) // texture coord translation
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
				else if (!strncmp(bufPtr, "-s", 2)) // texture coord scale
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
				else if (!strncmp(bufPtr, "-t", 2))
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
			// get next word
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		}

		if ((type == 1) && (core::isdigit(textureNameBuf[0])))
		{
			// materialParam = core::fast_atof(textureNameBuf);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		}

		if (clamp)
		{
			currMaterial->Meshbuffer->getMaterial().setFlag(video::EMF_TEXTURE_WRAP, video::ETC_CLAMP);
		}

		io::path texname(textureNameBuf);
		texname = relPath + texname;

		CShaderManager* shaderMgr = CShaderManager::getInstance();

		SMaterial& material = currMaterial->Meshbuffer->getMaterial();
		material.MaterialType = shaderMgr->getShaderIDByName("TextureColor");

		if (texname.size() > 0)
		{
			video::ITexture* texture = CTextureManager::getInstance()->getTexture(texname.c_str(), m_textureFolder);
			if (texture)
			{
				if (type == 0)
				{
					// diffuse map
					material.setTexture(0, texture);
				}
				else if (type == 1)
				{
					// normal map
					material.setTexture(1, texture);
				}
				else if (type == 2)
				{
					// alpha map
					material.setTexture(3, texture);
				}
				else if (type == 3)
				{
					// reflection map
					material.setTexture(4, texture);
				}
				else if (type == 4)
				{
					// specular map
					material.setTexture(2, texture);
				}
			}
		}

		return bufPtr;
	}

	//! skip space characters and stop on first non-space
	const c8* COBJMeshFileLoader::goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
	{
		// skip space characters
		if (acrossNewlines)
			while ((buf != bufEnd) && core::isspace(*buf))
				++buf;
		else
			while ((buf != bufEnd) && core::isspace(*buf) && (*buf != '\n'))
				++buf;

		return buf;
	}


	//! skip current word and stop at beginning of next one
	const c8* COBJMeshFileLoader::goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
	{
		// skip current word
		while ((buf != bufEnd) && !core::isspace(*buf))
			++buf;

		return goFirstWord(buf, bufEnd, acrossNewlines);
	}


	//! Read until line break is reached and stop at the next non-space character
	const c8* COBJMeshFileLoader::goNextLine(const c8* buf, const c8* const bufEnd)
	{
		// look for newline characters
		while (buf != bufEnd)
		{
			// found it, so leave
			if (*buf == '\n' || *buf == '\r')
				break;
			++buf;
		}
		return goFirstWord(buf, bufEnd);
	}


	u32 COBJMeshFileLoader::copyWord(c8* outBuf, const c8* const inBuf, u32 outBufLength, const c8* const bufEnd)
	{
		if (!outBufLength)
			return 0;
		if (!inBuf)
		{
			*outBuf = 0;
			return 0;
		}

		u32 i = 0;
		while (inBuf[i])
		{
			if (core::isspace(inBuf[i]) || &(inBuf[i]) == bufEnd)
				break;
			++i;
		}

		u32 length = core::min_(i, outBufLength - 1);
		for (u32 j = 0; j < length; ++j)
			outBuf[j] = inBuf[j];

		outBuf[length] = 0;
		return length;
	}


	core::stringc COBJMeshFileLoader::copyLine(const c8* inBuf, const c8* bufEnd)
	{
		if (!inBuf)
			return core::stringc();

		const c8* ptr = inBuf;
		while (ptr < bufEnd)
		{
			if (*ptr == '\n' || *ptr == '\r')
				break;
			++ptr;
		}
		// we must avoid the +1 in case the array is used up
		return core::stringc(inBuf, (u32)(ptr - inBuf + ((ptr < bufEnd) ? 1 : 0)));
	}


	const c8* COBJMeshFileLoader::goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* bufEnd)
	{
		inBuf = goNextWord(inBuf, bufEnd, false);
		copyWord(outBuf, inBuf, outBufLength, bufEnd);
		return inBuf;
	}

	//! Read RGB color
	const c8* COBJMeshFileLoader::readColor(const c8* bufPtr, video::SColor& color, const c8* const bufEnd)
	{
		const u32 COLOR_BUFFER_LENGTH = 16;
		c8 colStr[COLOR_BUFFER_LENGTH];

		color.setAlpha(255);
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setRed((s32)(core::fast_atof(colStr) * 255.0f));
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setGreen((s32)(core::fast_atof(colStr) * 255.0f));
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setBlue((s32)(core::fast_atof(colStr) * 255.0f));
		return bufPtr;
	}


	//! Read 3d vector of floats
	const c8* COBJMeshFileLoader::readVec3(const c8* bufPtr, core::vector3df& vec, const c8* const bufEnd)
	{
		c8 wordBuffer[WORD_BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.X = -core::fast_atof(wordBuffer); // change handedness
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Y = core::fast_atof(wordBuffer);
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Z = core::fast_atof(wordBuffer);
		return bufPtr;
	}


	//! Read 2d vector of floats
	const c8* COBJMeshFileLoader::readUV(const c8* bufPtr, core::vector2df& vec, const c8* const bufEnd)
	{
		c8 wordBuffer[WORD_BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.X = core::fast_atof(wordBuffer);
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Y = 1 - core::fast_atof(wordBuffer); // change handedness
		return bufPtr;
	}


	//! Read boolean value represented as 'on' or 'off'
	const c8* COBJMeshFileLoader::readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd)
	{
		const u32 BUFFER_LENGTH = 8;
		c8 tfStr[BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(tfStr, bufPtr, BUFFER_LENGTH, bufEnd);
		tf = strcmp(tfStr, "off") != 0;
		return bufPtr;
	}
}
