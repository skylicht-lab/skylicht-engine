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
#include "CMaterial.h"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"
#include "RenderMesh/CMesh.h"
#include "TextureManager/CTextureManager.h"
#include "Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CMaterial::CMaterial(const char* name, const char* shaderPath) :
		m_zBuffer(video::ECFN_LESSEQUAL),
		m_zWriteEnable(true),
		m_backfaceCulling(true),
		m_frontfaceCulling(false),
		m_doubleSided(false),
		m_manualInitMaterial(false),
		m_deferred(false),
		m_shaderPath(shaderPath),
		m_materialName(name),
		m_shader(NULL)
	{
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			m_resourceTexture[i] = NULL;
			m_textures[i] = NULL;
		}

		for (int i = 0; i < CShader::ResourceCount; i++)
			m_overrideTextures[i] = NULL;

		initMaterial();
	}

	CMaterial::~CMaterial()
	{
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			if (m_resourceTexture[i] != NULL)
				m_resourceTexture[i]->drop();

			m_resourceTexture[i] = NULL;
			m_textures[i] = NULL;
		}

		deleteAllParams();
		deleteExtraParams();

		if (CShaderMaterial::getMaterial() == this)
			CShaderMaterial::setMaterial(NULL);
	}

	CMaterial* CMaterial::clone()
	{
		CMaterial* mat = new CMaterial(m_materialName.c_str(), m_shaderPath.c_str());

		copyTo(mat);

		return mat;
	}

	void CMaterial::copyTo(CMaterial* mat)
	{
		mat->deleteAllParams();

		mat->m_shader = m_shader;
		mat->m_materialName = m_materialName;
		mat->m_shaderPath = m_shaderPath;
		mat->m_materialPath = m_materialPath;

		for (int i = 0; i < MAX_SHADERPARAMS; i++)
			mat->m_shaderParams.setValue(i, m_shaderParams.getParam(i));

		for (SUniformValue*& u : m_uniformParams)
		{
			SUniformValue* v = u->clone();
			mat->m_uniformParams.push_back(v);
		}

		for (SUniformTexture*& u : m_uniformTextures)
		{
			SUniformTexture* t = u->clone();

			if (t->Texture)
				t->Texture->grab();

			mat->m_uniformTextures.push_back(t);
		}

		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			mat->m_resourceTexture[i] = m_resourceTexture[i];

			if (mat->m_resourceTexture[i] != NULL)
				mat->m_resourceTexture[i]->grab();

			mat->m_textures[i] = m_textures[i];
		}

		mat->m_zBuffer = m_zBuffer;
		mat->m_zWriteEnable = m_zWriteEnable;
		mat->m_backfaceCulling = m_backfaceCulling;
		mat->m_frontfaceCulling = m_frontfaceCulling;
		mat->m_doubleSided = m_doubleSided;

		mat->m_deferred = m_deferred;

		mat->m_castShadow = m_castShadow;
		mat->m_manualInitMaterial = m_manualInitMaterial;
	}

	void CMaterial::copyParamsTo(CMaterial* mat)
	{
		for (SUniformValue*& uniform : mat->m_uniformParams)
			delete uniform;
		mat->m_uniformParams.clear();

		mat->m_shader = m_shader;
		mat->m_materialName = m_materialName;
		mat->m_shaderPath = m_shaderPath;
		mat->m_materialPath = m_materialPath;

		for (int i = 0; i < MAX_SHADERPARAMS; i++)
			mat->m_shaderParams.setValue(i, m_shaderParams.getParam(i));

		for (SUniformValue*& u : m_uniformParams)
		{
			SUniformValue* v = u->clone();
			mat->m_uniformParams.push_back(v);
		}

		mat->m_zBuffer = m_zBuffer;
		mat->m_zWriteEnable = m_zWriteEnable;
		mat->m_backfaceCulling = m_backfaceCulling;
		mat->m_frontfaceCulling = m_frontfaceCulling;
		mat->m_doubleSided = m_doubleSided;

		mat->m_deferred = m_deferred;

		mat->m_castShadow = m_castShadow;
		mat->m_manualInitMaterial = m_manualInitMaterial;
	}

	void CMaterial::deleteAllParams()
	{
		for (SUniformValue*& uniform : m_uniformParams)
			delete uniform;
		m_uniformParams.clear();

		for (SUniformTexture*& uniform : m_uniformTextures)
		{
			if (uniform->Texture)
				uniform->Texture->drop();
			delete uniform;
		}
		m_uniformTextures.clear();
	}

	void CMaterial::deleteExtraParams()
	{
		for (SExtraParams*& e : m_extras)
		{
			for (SUniformValue*& uniform : e->UniformParams)
				delete uniform;
			e->UniformParams.clear();

			for (SUniformTexture*& uniform : e->UniformTextures)
			{
				if (uniform->Texture)
					uniform->Texture->drop();
				delete uniform;
			}
			e->UniformTextures.clear();

			delete e;
		}

		m_extras.clear();
	}

	void CMaterial::setUniform(const char* name, float f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			p->FloatValue[0] = f;
		}
	}

	void CMaterial::setUniform2(const char* name, float* f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 2);
		}
	}

	void CMaterial::setUniform3(const char* name, float* f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 3);
		}
	}

	void CMaterial::setUniform4(const char* name, float* f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 4);
		}
	}

	void CMaterial::setUniform4(const char* name, const SColor& color)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			SColorf c(color);
			float f[] = { c.r, c.g, c.b, c.a };
			memcpy(p->FloatValue, f, sizeof(float) * 4);
		}
	}

	const char* CMaterial::getUniformTextureName(int slot)
	{
		if (slot >= 0 && slot < (int)m_uniformTextures.size())
			return m_uniformTextures[slot]->Name.c_str();
		return NULL;
	}

	void CMaterial::setUniformTexture(const char* name, ITexture* texture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL && texture != NULL)
		{
			if (p->Texture)
				p->Texture->drop();
			p->Texture = texture;
			if (p->Texture)
				p->Texture->grab();
		}
	}

	void CMaterial::setUniformTextureWrapU(const char* name, int wrapU)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			p->WrapU = wrapU;
		}
	}

	void CMaterial::setUniformTextureWrapV(const char* name, int wrapV)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			p->WrapV = wrapV;
		}
	}

	void CMaterial::setUniformTexture(const char* name, const char* path, bool loadTexture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			p->Path = path;

			if (loadTexture == true)
			{
				if (p->Texture)
					p->Texture->drop();
				p->Texture = CTextureManager::getInstance()->getTexture(path);
				if (p->Texture)
					p->Texture->grab();
			}
		}
	}

	void CMaterial::setUniformTexture(const char* name, const char* path, std::vector<std::string>& folder, bool loadTexture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			CTextureManager* textureManager = CTextureManager::getInstance();
			p->Path = path;
			p->Texture = NULL;

			if (textureManager->existTexture(path) == true)
			{
				if (loadTexture == true)
				{
					if (p->Texture)
						p->Texture->drop();
					p->Texture = CTextureManager::getInstance()->getTexture(path);
					if (p->Texture)
						p->Texture->grab();
				}
			}
			else
			{
				// try search in folder
				for (int i = 0, n = (int)folder.size(); i < n; i++)
				{
					std::string s = folder[i];
					s += "/";
					s += path;

					if (textureManager->existTexture(s.c_str()) == true)
					{
						if (loadTexture == true)
						{
							if (p->Texture)
								p->Texture->drop();
							p->Texture = CTextureManager::getInstance()->getTexture(s.c_str());
							if (p->Texture)
								p->Texture->grab();
						}
						break;
					}
				}
			}

			if (p->TextureSlot >= 0 && p->TextureSlot < MATERIAL_MAX_TEXTURES)
				m_textures[p->TextureSlot] = p->Texture;
		}
	}

	CMaterial::SExtraParams* CMaterial::newExtra(const char* shaderPath)
	{
		SExtraParams* e = new SExtraParams();
		e->ShaderPath = shaderPath;

		// insert at front of list
		// that priorty to search old params
		m_extras.insert(m_extras.begin(), e);
		return e;
	}

	void CMaterial::setExtraUniformTexture(SExtraParams* e, const char* name, const char* path)
	{
		SUniformTexture* t = getExtraUniformTexture(e, name);
		t->Path = path;
	}

	void CMaterial::setExtraUniformTextureWrapU(SExtraParams* e, const char* name, int wrapU)
	{
		SUniformTexture* t = getExtraUniformTexture(e, name);
		t->WrapU = wrapU;
	}

	void CMaterial::setExtraUniformTextureWrapV(SExtraParams* e, const char* name, int wrapV)
	{
		SUniformTexture* t = getExtraUniformTexture(e, name);
		t->WrapV = wrapV;
	}

	void CMaterial::setExtraUniform(SExtraParams* e, const char* name, float* f, int floatSize)
	{
		SUniformValue* t = getExtraUniform(e, name);
		t->FloatSize = floatSize;
		t->FloatValue[0] = f[0];
		t->FloatValue[1] = f[1];
		t->FloatValue[2] = f[2];
		t->FloatValue[3] = f[3];
	}

	CMaterial::SUniformValue* CMaterial::getUniform(const char* name)
	{
		for (SUniformValue* uniform : m_uniformParams)
		{
			if (uniform->Name == name)
				return uniform;
		}

		return newUniform(name, 4);
	}

	bool CMaterial::haveUniform(const char* name)
	{
		for (SUniformValue* uniform : m_uniformParams)
		{
			if (uniform->Name == name)
				return true;
		}

		return false;
	}

	CMaterial::SUniformTexture* CMaterial::getUniformTexture(const char* name)
	{
		for (SUniformTexture* uniform : m_uniformTextures)
		{
			if (uniform->Name == name)
				return uniform;
		}

		return newUniformTexture(name);
	}

	CMaterial::SUniformValue* CMaterial::getExtraUniform(SExtraParams* e, const char* name)
	{
		for (SUniformValue* uniform : e->UniformParams)
		{
			if (uniform->Name == name)
				return uniform;
		}

		return newExtraUniform(e, name, 4);
	}

	CMaterial::SUniformTexture* CMaterial::getExtraUniformTexture(SExtraParams* e, const char* name)
	{
		for (SUniformTexture* uniform : e->UniformTextures)
		{
			if (uniform->Name == name)
				return uniform;
		}

		return newExtraUniformTexture(e, name);
	}

	std::string CMaterial::getProperty(const std::string& name)
	{
		if (name == "ZBuffer")
		{
			if (m_zBuffer == video::ECFN_ALWAYS)
				return "Always";
			else if (m_zBuffer == video::ECFN_DISABLED)
				return "Disabled";
			else if (m_zBuffer == video::ECFN_LESSEQUAL)
				return "LessEqual";
			else if (m_zBuffer == video::ECFN_GREATER)
				return "Greater";
		}
		else if (name == "ZWriteEnable")
		{
			return m_zWriteEnable ? "true" : "false";
		}
		else if (name == "BackfaceCulling")
		{
			return m_backfaceCulling ? "true" : "false";
		}
		else if (name == "FrontfaceCulling")
		{
			return m_frontfaceCulling ? "true" : "false";
		}
		else if (name == "DoubleSided")
		{
			return m_doubleSided ? "true" : "false";
		}

		return "";
	}

	void CMaterial::setProperty(const std::string& name, const std::string& value)
	{
		bool booleanValue = false;

		std::string data = value;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		if (data == "true")
			booleanValue = true;

		if (name == "ZBuffer")
		{
			if (value == "Always")
				m_zBuffer = video::ECFN_ALWAYS;
			else if (value == "Disabled")
				m_zBuffer = video::ECFN_DISABLED;
			else if (value == "LessEqual")
				m_zBuffer = video::ECFN_LESSEQUAL;
			else if (value == "Greater")
				m_zBuffer = video::ECFN_GREATER;
		}
		else if (name == "ZWriteEnable")
		{
			m_zWriteEnable = booleanValue;
		}
		else if (name == "BackfaceCulling")
		{
			m_backfaceCulling = booleanValue;
		}
		else if (name == "FrontfaceCulling")
		{
			m_frontfaceCulling = booleanValue;
		}
		else if (name == "DoubleSided")
		{
			m_doubleSided = booleanValue;
		}
	}

	void CMaterial::setTexture(ITexture** textures, int num)
	{
		for (int i = 0; i < num; i++)
			m_textures[i] = textures[i];

		m_manualInitMaterial = true;
		updateSetTextureSlot();
	}

	void CMaterial::setTexture(int slot, ITexture* texture)
	{
		m_textures[slot] = texture;

		m_manualInitMaterial = true;
		updateSetTextureSlot();
	}

	ITexture* CMaterial::getTexture(int slot)
	{
		return m_textures[slot];
	}

	void CMaterial::loadUniformTexture()
	{
		if (m_shader == NULL)
			return;

		CTextureManager* textureManager = CTextureManager::getInstance();

		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture* uniformTexture = m_uniformTextures[i];
			SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());

			if (uniform != NULL)
			{
				int textureSlot = (int)uniform->Value[0];
				if (textureSlot < MATERIAL_MAX_TEXTURES)
				{
					if (uniformTexture->Path.empty() == false)
					{
						ITexture* texture = textureManager->getTexture(uniformTexture->Path.c_str());
						m_textures[textureSlot] = texture;
						uniformTexture->Texture = texture;
						if (texture)
							texture->grab();
					}
				}
			}
		}
	}

	void CMaterial::loadDefaultTexture()
	{
		CTextureManager* textureManager = CTextureManager::getInstance();

		if (m_shader != NULL)
		{
			for (int i = 0, n = m_shader->getNumResource(); i < n; i++)
			{
				CShader::SResource* r = m_shader->getResouceInfo(i);

				SUniform* uniform = m_shader->getFSUniform(r->Name.c_str());
				if (uniform != NULL)
				{
					int textureSlot = (int)uniform->Value[0];
					ITexture*& texture = m_resourceTexture[textureSlot];

					// drop current holder
					if (texture != NULL)
					{
						texture->drop();
						texture = NULL;
					}

					// we will custom override texture first
					ITexture* overrideTexture = m_overrideTextures[r->Type];
					if (overrideTexture != NULL)
					{
						texture = overrideTexture;
					}
					else
					{
						if (r->Type == CShader::Texture)
						{
							texture = textureManager->getTexture(r->Path.c_str());
						}
						else if (r->Type == CShader::CubeTexture)
						{
							std::string path = r->Path;
							CStringImp::replaceAll(path, "_X1.png", "");

							std::string x1 = std::string(path) + "_X1.png";
							std::string x2 = std::string(path) + "_X2.png";
							std::string y1 = std::string(path) + "_Y1.png";
							std::string y2 = std::string(path) + "_Y2.png";
							std::string z1 = std::string(path) + "_Z1.png";
							std::string z2 = std::string(path) + "_Z2.png";

							texture = textureManager->getCubeTexture(
								x1.c_str(), x2.c_str(),
								y1.c_str(), y2.c_str(),
								z1.c_str(), z2.c_str());
						}
						else
						{
							// see function CBaseRP::updateShaderResource
							// auto attach some realtime texture resource like
							// ShadowMap, Reflection...
						}
					}

					// hold this texture
					if (texture != NULL)
						texture->grab();
				}
			}
		}
	}

	void CMaterial::unloadDefaultTexture()
	{
		CTextureManager* textureManager = CTextureManager::getInstance();

		if (m_shader != NULL)
		{
			for (int i = 0, n = m_shader->getNumResource(); i < n; i++)
			{
				CShader::SResource* r = m_shader->getResouceInfo(i);

				SUniform* uniform = m_shader->getFSUniform(r->Name.c_str());
				if (uniform != NULL)
				{
					int textureSlot = (int)uniform->Value[0];
					ITexture*& texture = m_resourceTexture[textureSlot];

					// drop current holder
					if (texture != NULL)
					{
						texture->drop();
						texture = NULL;
					}
				}
			}
		}
	}

	void CMaterial::unloadUniformTexture()
	{
		if (m_shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture* uniformTexture = m_uniformTextures[i];
			SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());

			if (uniform != NULL)
			{
				int textureSlot = (int)uniform->Value[0];
				if (textureSlot < MATERIAL_MAX_TEXTURES && m_textures[textureSlot])
				{
					m_textures[textureSlot]->drop();
					m_textures[textureSlot] = NULL;
					uniformTexture->Texture = NULL;
				}
			}
		}
	}

	void CMaterial::unloadUniformTexture(const char* name)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL && p->Texture)
		{
			p->Texture->drop();
			p->Texture = NULL;
			m_textures[p->TextureSlot] = NULL;
		}
	}

	void CMaterial::initMaterial()
	{
		deleteAllParams();

		CShaderManager* shaderManager = CShaderManager::getInstance();
		m_shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (m_shader == NULL)
			m_shader = shaderManager->loadShader(m_shaderPath.c_str());

		if (m_shader != NULL)
		{
			// check pipeline
			m_deferred = m_shader->isDeferred();

			// add gui
			int numUI = m_shader->getNumUI();
			for (int i = 0; i < numUI; i++)
			{
				CShader::SUniformUI* ui = m_shader->getUniformUI(i);
				if (ui->ControlType == CShader::UIGroup)
				{
					for (int j = 0, n = ui->Childs.size(); j < n; j++)
					{
						addShaderUI(ui->Childs[j]);
					}
				}
				else
				{
					addShaderUI(ui);
				}
			}
		}

		loadDefaultTexture();

		initDefaultValue();

		bindUniformParam();

		updateShaderParams();
	}

	// PUBLIC FOR USE

	void CMaterial::applyMaterial(SMaterial& mat)
	{
		loadDefaultTexture();

		initDefaultValue();

		updateShaderParams();

		updateTexture(mat);
	}

	void CMaterial::applyMaterial()
	{
		loadDefaultTexture();

		initDefaultValue();

		updateShaderParams();
	}

	void CMaterial::changeShader(CShader* shader)
	{
		// save current shader params
		saveExtraParams();

		// new shader path
		m_shaderPath = shader->getSource();

		// init default params
		initMaterial();

		// try load from extra params
		reloadExtraParams(m_shaderPath.c_str());

		// update params
		updateShaderParams();
	}

	void CMaterial::changeShader(const char* path)
	{
		CShaderManager* shaderManager = CShaderManager::getInstance();
		CShader* shader = shaderManager->getShaderByPath(path);
		if (shader != NULL)
		{
			changeShader(shader);
		}
		else
		{
			// try load shader
			CShader* shader = shaderManager->loadShader(path);
			if (shader != NULL)
			{
				changeShader(shader);
			}
		}
	}

	bool CMaterial::autoDetectLoadTexture()
	{
		bool ret = true;

		CTextureManager* textureManager = CTextureManager::getInstance();

		if (m_shader != NULL)
		{
			std::vector<std::string> texNamePaths;
			std::vector<std::string> texExtPaths;

			std::vector<std::string> paths;
			std::vector<std::string> exts;

			for (SUniformTexture* texture : m_uniformTextures)
			{
				CShader::SUniformUI* ui = m_shader->getUniformUIByName(texture->Name.c_str());
				if (ui == NULL)
					continue;

				if (texture->Texture != NULL)
				{
					// add references
					const char* path = texture->Texture->getName().getPath().c_str();
					for (std::string& s : ui->AutoReplace)
					{
						if (strstr(path, s.c_str()) != NULL)
						{
							paths.push_back(path);
							exts.push_back(s);
						}
					}

					std::string folder = CPath::getFolderPath(path);
					std::string name = CPath::getFileNameNoExt(path);
					std::string ext = CPath::getFileNameExt(path);

					texNamePaths.push_back(folder + "/" + name);
					texExtPaths.push_back(ext);
				}
				else
				{
					// need try to fill and load texture
					if (ui != NULL && ui->AutoReplace.size() > 0)
					{
						bool found = false;
						std::string foundPath;

						for (std::string& s : ui->AutoReplace)
						{
							found = false;
							for (u32 i = 0, n = (u32)paths.size(); i < n; i++)
							{
								char t[512] = { 0 };
								CStringImp::replaceText(
									t,
									paths[i].c_str(),
									exts[i].c_str(),
									s.c_str());

								if (textureManager->existTexture(t) == true)
								{
									paths.push_back(t);
									exts.push_back(s);

									foundPath = t;
									found = true;
									break;
								}
							}

							if (found == true)
								break;
						}

						// try test again
						if (found == false)
						{
							for (std::string& s : ui->AutoReplace)
							{
								for (u32 i = 0, n = (u32)texNamePaths.size(); i < n; i++)
								{
									std::string fileName = texNamePaths[i];
									fileName += s;
									fileName += texExtPaths[i];

									if (textureManager->existTexture(fileName.c_str()) == true)
									{
										foundPath = fileName;
										found = true;
										break;
									}
								}

								if (found == true)
									break;
							}
						}

						if (found == true)
						{
							ITexture* texture = textureManager->getTexture(foundPath.c_str());
							if (texture != NULL)
								setUniformTexture(ui->Name.c_str(), texture);
						}
						else
						{
							char log[512];
							sprintf(log, "[CMaterial::autoDetectLoadTexture] %s: not found texture uniform %s", m_materialName.c_str(), texture->Name.c_str());
							os::Printer::log(log);
							if (ui->UniformInfo)
							{
								// todo replace empty texture
								if (ui->UniformInfo->IsNormal == true)
									setUniformTexture(ui->Name.c_str(), textureManager->getNullNormalMap());
								else
									setUniformTexture(ui->Name.c_str(), textureManager->getNullTexture());
							}
							ret = false;
						}
					}
				}
			}
		}

		// apply to texture slot
		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture* uniformTexture = m_uniformTextures[i];
			if (uniformTexture->TextureSlot == -1)
			{
				SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());
				if (uniform != NULL)
					uniformTexture->TextureSlot = (int)uniform->Value[0];
				else
					uniformTexture->TextureSlot = -2;
			}

			int textureSlot = uniformTexture->TextureSlot;
			if (textureSlot >= 0 && textureSlot < MATERIAL_MAX_TEXTURES)
				m_textures[textureSlot] = m_uniformTextures[i]->Texture;
		}

		return ret;
	}

	void CMaterial::initDefaultValue()
	{
		if (m_shader != NULL)
		{
			int numVS = m_shader->getNumVS();
			for (int i = 0; i < numVS; i++)
			{
				SUniform* u = m_shader->getVSUniformID(i);
				if (u->Type == MATERIAL_PARAM)
				{
					if (haveUniform(u->Name.c_str()) == false)
					{
						// need config default value from shader (first-time)
						SUniformValue* v = getUniform(u->Name.c_str());
						setDefaultValue(v, u);
					}
				}
			}

			int numFS = m_shader->getNumFS();
			for (int i = 0; i < numFS; i++)
			{
				SUniform* u = m_shader->getFSUniformID(i);
				if (u->Type == MATERIAL_PARAM)
				{
					if (haveUniform(u->Name.c_str()) == false)
					{
						// need config default value from shader (first-time)
						SUniformValue* v = getUniform(u->Name.c_str());
						setDefaultValue(v, u);
					}
				}
			}
		}
	}

	CMaterial::SUniformValue* CMaterial::newUniform(const char* name, int floatSize)
	{
		SUniformValue* p = new SUniformValue();
		p->Name = name;
		p->FloatSize = floatSize;
		m_uniformParams.push_back(p);
		return p;
	}

	CMaterial::SUniformTexture* CMaterial::newUniformTexture(const char* name)
	{
		SUniformTexture* p = new SUniformTexture();
		p->Name = name;
		p->Texture = NULL;
		m_uniformTextures.push_back(p);
		return p;
	}

	CMaterial::SUniformTexture* CMaterial::newExtraUniformTexture(SExtraParams* e, const char* name)
	{
		SUniformTexture* p = new SUniformTexture();
		p->Name = name;

		e->UniformTextures.push_back(p);
		return p;
	}

	CMaterial::SUniformValue* CMaterial::newExtraUniform(SExtraParams* e, const char* name, int floatSize)
	{
		SUniformValue* p = new SUniformValue();
		p->Name = name;
		p->FloatSize = floatSize;

		e->UniformParams.push_back(p);
		return p;
	}

	void CMaterial::updateTexture(SMaterial& mat)
	{
		// property
		mat.ZBuffer = m_zBuffer;
		mat.ZWriteEnable = m_zWriteEnable;
		mat.BackfaceCulling = m_backfaceCulling;
		mat.FrontfaceCulling = m_frontfaceCulling;

		// apply default filter for all texture layers
		mat.UseMipMaps = true;
		mat.setFlag(video::EMF_BILINEAR_FILTER, false);
		mat.setFlag(video::EMF_TRILINEAR_FILTER, false);
		mat.setFlag(video::EMF_ANISOTROPIC_FILTER, true, 2);

		// apply default shader material
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			if (m_resourceTexture[i] != NULL)
			{
				mat.setTexture(i, m_resourceTexture[i]);
			}
		}

		// apply config material
		if (m_shader != NULL)
		{
			mat.MaterialType = m_shader->getMaterialRenderID();

			if (mat.MaterialType == -1)
				mat.MaterialType = 0;

			// manual init
			if (m_manualInitMaterial == true)
			{
				for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
				{
					if (m_textures[i] != NULL)
						mat.setTexture(i, m_textures[i]);
				}
			}

			// auto uniform config
			for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
			{
				SUniformTexture* uniformTexture = m_uniformTextures[i];
				if (uniformTexture->TextureSlot == -1)
				{
					SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());
					if (uniform != NULL)
						uniformTexture->TextureSlot = (int)uniform->Value[0];
					else
					{
						uniform = m_shader->getVSUniform(uniformTexture->Name.c_str());
						if (uniform != NULL)
							uniformTexture->TextureSlot = (int)uniform->Value[0];
						else
							uniformTexture->TextureSlot = -2;
					}
				}

				int textureSlot = uniformTexture->TextureSlot;
				if (textureSlot >= 0 && textureSlot < MATERIAL_MAX_TEXTURES)
				{
					m_textures[textureSlot] = uniformTexture->Texture;

					// set customize texture
					mat.setTexture(textureSlot, uniformTexture->Texture);

					// set unwrap
					mat.TextureLayer[textureSlot].TextureWrapU = uniformTexture->WrapU;
					mat.TextureLayer[textureSlot].TextureWrapV = uniformTexture->WrapV;

					// set customize filter
					mat.TextureLayer[textureSlot].BilinearFilter = uniformTexture->Bilinear;
					mat.TextureLayer[textureSlot].TrilinearFilter = uniformTexture->Trilinear;
					mat.TextureLayer[textureSlot].AnisotropicFilter = uniformTexture->Anisotropic;
				}
			}
		}
	}

	void CMaterial::replaceTexture(ITexture* oldTexture, ITexture* newTexture)
	{
		if (oldTexture == NULL || newTexture == NULL)
			return;

		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			if (m_resourceTexture[i] == oldTexture)
			{
				m_resourceTexture[i]->drop();
				m_resourceTexture[i] = newTexture;
				m_resourceTexture[i]->grab();
			}

			if (m_textures[i] == oldTexture)
				m_textures[i] = newTexture;
		}

		for (SUniformTexture*& uniform : m_uniformTextures)
		{
			if (uniform->Texture == oldTexture)
			{
				uniform->Texture->drop();
				uniform->Texture = newTexture;
				uniform->Texture->grab();
			}
		}

		for (SExtraParams*& e : m_extras)
		{
			for (SUniformTexture*& uniform : e->UniformTextures)
			{
				if (uniform->Texture == oldTexture)
				{
					uniform->Texture->drop();
					uniform->Texture = newTexture;
					uniform->Texture->grab();
				}
			}
		}
	}

	void CMaterial::bindUniformParam()
	{
		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture* uniformTexture = m_uniformTextures[i];
			if (uniformTexture->TextureSlot == -1)
			{
				SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());
				if (uniform != NULL)
					uniformTexture->TextureSlot = (int)uniform->Value[0];
				else
					uniformTexture->TextureSlot = -2;
			}
		}

		for (int i = 0, n = (int)m_uniformParams.size(); i < n; i++)
		{
			SUniformValue* uniformValue = m_uniformParams[i];

			if (uniformValue->Type == NUM_SHADER_TYPE)
			{
				SUniform* uniform = m_shader->getFSUniform(uniformValue->Name.c_str());
				if (uniform == NULL)
					uniform = m_shader->getVSUniform(uniformValue->Name.c_str());

				if (uniform != NULL)
				{
					uniformValue->Type = uniform->Type;
					uniformValue->ValueIndex = uniform->ValueIndex;
				}
			}
		}
	}

	void CMaterial::updateSetTextureSlot()
	{
		if (m_shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture* uniformTexture = m_uniformTextures[i];
			if (uniformTexture->TextureSlot == -1)
			{
				SUniform* uniform = m_shader->getFSUniform(uniformTexture->Name.c_str());
				if (uniform != NULL)
					uniformTexture->TextureSlot = (int)uniform->Value[0];
				else
					uniformTexture->TextureSlot = -2;
			}

			if (uniformTexture->TextureSlot >= 0 && uniformTexture->TextureSlot < MATERIAL_MAX_TEXTURES)
			{
				ITexture* texture = m_textures[uniformTexture->TextureSlot];
				if (texture)
				{
					if (uniformTexture->Texture)
						uniformTexture->Texture->drop();

					uniformTexture->Texture = texture;

					if (uniformTexture->Texture)
						uniformTexture->Texture->grab();
				}
			}
		}
	}

	void CMaterial::updateShaderParams()
	{
		if (m_shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformParams.size(); i < n; i++)
		{
			SUniformValue* uniformValue = m_uniformParams[i];

			if (uniformValue->Type == NUM_SHADER_TYPE)
			{
				// unknow type
				SUniform* uniform = m_shader->getFSUniform(uniformValue->Name.c_str());
				if (uniform == NULL)
					uniform = m_shader->getVSUniform(uniformValue->Name.c_str());

				if (uniform != NULL)
				{
					uniformValue->Type = uniform->Type;
					uniformValue->ValueIndex = uniform->ValueIndex;
				}
			}

			switch (uniformValue->Type)
			{
			case MATERIAL_PARAM:
			{
				if (uniformValue->ValueIndex >= 0 && uniformValue->ValueIndex < MAX_SHADERPARAMS)
				{
					SVec4& v = m_shaderParams.getParam(uniformValue->ValueIndex);
					v.X = uniformValue->FloatValue[0];
					v.Y = uniformValue->FloatValue[1];
					v.Z = uniformValue->FloatValue[2];
					v.W = uniformValue->FloatValue[3];
					uniformValue->ShaderDefaultValue = false;
				}
				else
				{
					char log[512];
					sprintf(log, "[CMaterial] - Warning!!! Fail to apply shader param '%s' type:MATERIAL_PARAM", uniformValue->Name.c_str());
					os::Printer::log(log);
					break;
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}

	void CMaterial::addShaderUI(CShader::SUniformUI* ui)
	{
		if (ui->ControlType == CShader::UITexture)
		{
			// add uniform texture
			newUniformTexture(ui->Name.c_str());
		}
		else
		{
			if (ui->UniformInfo)
			{
				if (ui->ControlType == CShader::UIColor)
				{
					// add color uniform
					newUniform(ui->Name.c_str(), ui->UniformInfo->FloatSize);
				}

				// apply default shader value
				setUniform4(ui->Name.c_str(), ui->UniformInfo->Value);
			}
		}
	}

	void CMaterial::setDefaultValue(SUniformValue* v, SUniform* u)
	{
		memcpy(v->FloatValue, u->Value, sizeof(float) * 4);

		v->ShaderDefaultValue = true;
	}

	CMaterial::SExtraParams* CMaterial::getExtraParams(const char* shaderPath)
	{
		for (SExtraParams* e : m_extras)
		{
			if (e->ShaderPath == shaderPath)
				return e;
		}

		return NULL;
	}

	void CMaterial::saveExtraParams()
	{
		// get or create extra
		SExtraParams* e = getExtraParams(m_shaderPath.c_str());
		if (e == NULL)
			e = newExtra(m_shaderPath.c_str());
		else
		{
			// move extra to front
			// that priorty to search old params
			auto it = m_extras.begin();
			while (it != m_extras.end())
			{
				if ((*it) == e)
				{
					m_extras.erase(it);
					break;
				}
				++it;
			}
			m_extras.insert(m_extras.begin(), e);
		}

		// clear old params
		for (SUniformValue*& uniform : e->UniformParams)
			delete uniform;
		e->UniformParams.clear();

		for (SUniformTexture*& uniform : e->UniformTextures)
		{
			if (uniform->Texture)
				uniform->Texture->drop();
			delete uniform;
		}
		e->UniformTextures.clear();

		// add current params
		for (SUniformTexture*& t : m_uniformTextures)
		{
			SUniformTexture* uniform = t->clone();
			if (uniform->Texture)
				uniform->Texture->grab();
			e->UniformTextures.push_back(uniform);
		}

		for (SUniformValue*& v : m_uniformParams)
		{
			e->UniformParams.push_back(v->clone());
		}
	}

	void CMaterial::reloadExtraParams(const char* shaderPath)
	{
		SExtraParams* e = getExtraParams(m_shaderPath.c_str());
		if (e == NULL)
		{
			// no extra data
			// try find in same shader params
			for (SUniformTexture*& t : m_uniformTextures)
			{
				if (t->Texture != NULL)
					continue;

				SUniformTexture* uniform = findExtraTexture(t->Name.c_str());
				if (uniform != NULL)
				{
					t->Path = uniform->Path;
					if (t->Texture)
						t->Texture->drop();
					t->Texture = uniform->Texture;
					if (t->Texture)
						t->Texture->grab();
				}
			}

			for (SUniformValue*& v : m_uniformParams)
			{
				SUniformValue* uniform = findExtraParam(v->Name.c_str(), v->FloatSize);
				if (uniform != NULL)
				{
					v->FloatValue[0] = uniform->FloatValue[0];
					v->FloatValue[1] = uniform->FloatValue[1];
					v->FloatValue[2] = uniform->FloatValue[2];
					v->FloatValue[3] = uniform->FloatValue[3];
				}
			}
		}
		else
		{
			// copy from extra data
			for (SUniformTexture*& t : m_uniformTextures)
			{
				if (t->Texture != NULL)
					continue;

				for (SUniformTexture*& uniform : e->UniformTextures)
				{
					if (t->Name == uniform->Name)
					{
						t->Path = uniform->Path;
						t->Texture = uniform->Texture;

						if (t->Texture == NULL && !t->Path.empty())
							t->Texture = CTextureManager::getInstance()->getTexture(t->Path.c_str());

						if (t->Texture)
							t->Texture->grab();

						break;
					}
				}
			}

			for (SUniformValue*& v : m_uniformParams)
			{
				for (SUniformValue*& uniform : e->UniformParams)
				{
					if (v->Name == uniform->Name && v->FloatSize == uniform->FloatSize)
					{
						v->FloatValue[0] = uniform->FloatValue[0];
						v->FloatValue[1] = uniform->FloatValue[1];
						v->FloatValue[2] = uniform->FloatValue[2];
						v->FloatValue[3] = uniform->FloatValue[3];
						break;
					}
				}
			}
		}
	}

	CMaterial::SUniformTexture* CMaterial::findExtraTexture(const char* name)
	{
		for (SExtraParams* e : m_extras)
		{
			for (CMaterial::SUniformTexture* t : e->UniformTextures)
			{
				if (t->Name == name)
				{
					if (t->Texture == NULL && !t->Path.empty())
					{
						t->Texture = CTextureManager::getInstance()->getTexture(t->Path.c_str());
						if (t->Texture)
							t->Texture->grab();
					}

					if (t->Texture != NULL)
						return t;
				}
			}
		}

		return NULL;
	}

	CMaterial::SUniformValue* CMaterial::findExtraParam(const char* name, int floatSize)
	{
		for (SExtraParams* e : m_extras)
		{
			for (CMaterial::SUniformValue* v : e->UniformParams)
			{
				if (v->Name == name && v->FloatSize == floatSize)
					return v;
			}
		}

		return NULL;
	}
}