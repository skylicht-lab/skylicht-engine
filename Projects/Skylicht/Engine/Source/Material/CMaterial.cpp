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

namespace Skylicht
{
	CMaterial::CMaterial(CGameObject *gameObject, const char *name, const char *shaderPath) :
		m_owner(gameObject),
		m_zBuffer(video::ECFN_LESSEQUAL),
		m_zWriteEnable(true),
		m_backfaceCulling(true),
		m_frontfaceCulling(false),
		m_doubleSided(false),
		m_manualInitMaterial(false),
		m_deferred(false),
		m_shadowMapTextureSlot(-1),
		m_shaderPath(shaderPath),
		m_materialName(name)
	{
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			m_resourceTexture[i] = NULL;
			m_textures[i] = NULL;
		}

		for (int i = 0; i < CShader::RResourceCount; i++)
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
		clearAllAffectMesh();
	}

	CMaterial* CMaterial::clone(CGameObject *gameObject)
	{
		CMaterial *mat = new CMaterial(gameObject, m_materialName.c_str(), m_shaderPath.c_str());
		mat->deleteAllParams();

		for (SUniformValue *&u : m_uniformParams)
		{
			SUniformValue* v = u->clone();
			mat->m_uniformParams.push_back(v);
		}

		for (SUniformTexture *&u : m_uniformTextures)
		{
			SUniformTexture* t = u->clone();
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

		return mat;
	}

	void CMaterial::deleteAllParams()
	{
		for (SUniformValue *&uniform : m_uniformParams)
			delete uniform;
		m_uniformParams.clear();

		for (SUniformTexture *&uniform : m_uniformTextures)
			delete uniform;
		m_uniformTextures.clear();
	}

	void CMaterial::setUniform(const char *name, float f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			p->FloatValue[0] = f;
		}
	}

	void CMaterial::setUniform2(const char *name, float *f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 2);
		}
	}

	void CMaterial::setUniform3(const char *name, float *f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 3);
		}
	}

	void CMaterial::setUniform4(const char *name, float *f)
	{
		SUniformValue* p = getUniform(name);
		if (p != NULL)
		{
			memcpy(p->FloatValue, f, sizeof(float) * 4);
		}
	}

	void CMaterial::setUniformTexture(const char *name, ITexture *texture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			p->Path = texture->getName().getPath().c_str();
			p->Texture = texture;
		}
	}

	void CMaterial::setUniformTexture(const char *name, const char *path, bool loadTexture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			p->Path = path;

			if (loadTexture == true)
				p->Texture = CTextureManager::getInstance()->getTexture(path);
		}
	}

	void CMaterial::setUniformTexture(const char *name, const char *path, std::vector<std::string>& folder, bool loadTexture)
	{
		SUniformTexture* p = getUniformTexture(name);
		if (p != NULL)
		{
			io::IFileSystem *fs = getIrrlichtDevice()->getFileSystem();

			if (fs->existFile(path) == true)
			{
				p->Path = path;

				if (loadTexture == true)
					p->Texture = CTextureManager::getInstance()->getTexture(path);
			}
			else
			{
				p->Path = path;
				p->Texture = NULL;

				const char *exts[] = { ".pvr", ".etc", ".dds" };

				for (int i = 0, n = (int)folder.size(); i < n; i++)
				{
					std::string s = folder[i];
					s += "/";
					s += path;

					bool fileExist = false;

					if (fs->existFile(s.c_str()) == true)
						fileExist = true;
					else
					{
						for (int j = 0; j < 3; j++)
						{
							s = CPath::replaceFileExt(s, std::string(exts[j]));
							if (fs->existFile(s.c_str()) == true)
							{
								fileExist = true;
								break;
							}
						}
					}

					if (fileExist == true)
					{
						p->Path = s;
						if (loadTexture == true)
							p->Texture = CTextureManager::getInstance()->getTexture(s.c_str());
						break;
					}
				}
			}
		}
	}

	CMaterial::SUniformValue* CMaterial::getUniform(const char *name)
	{
		for (int i = 0, n = (int)m_uniformParams.size(); i < n; i++)
		{
			if (m_uniformParams[i]->Name == name)
				return m_uniformParams[i];
		}

		return newUniform(name, 4);
	}

	bool CMaterial::haveUniform(const char *name)
	{
		for (int i = 0, n = (int)m_uniformParams.size(); i < n; i++)
		{
			if (m_uniformParams[i]->Name == name)
				return true;
		}

		return false;
	}

	CMaterial::SUniformTexture* CMaterial::getUniformTexture(const char *name)
	{
		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			if (m_uniformTextures[i]->Name == name)
				return m_uniformTextures[i];
		}

		return newUniformTexture(name);
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
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
			m_textures[i] = textures[i];

		updateShaderTexture();
	}

	void CMaterial::buildDoubleSidedMesh()
	{
		if (m_doubleSided == true)
		{
			for (int i = 0, n = (int)m_meshBuffers.size(); i < n; i++)
			{
				IMeshBuffer *buffer = m_meshBuffers[i];
				CMesh::applyDoubleSided(buffer);
			}
		}
	}

	void CMaterial::loadUniformTexture()
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		CTextureManager *textureManager = CTextureManager::getInstance();

		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture *textureUI = m_uniformTextures[i];

			CShader::SUniform* uniform = shader->getFSUniform(textureUI->Name.c_str());
			if (uniform != NULL)
			{
				int textureSlot = (int)uniform->Value[0];
				if (textureSlot < MATERIAL_MAX_TEXTURES)
				{
					if (textureUI->Path.empty() == false)
					{
						ITexture *texture = textureManager->getTexture(textureUI->Path.c_str());
						m_textures[textureSlot] = texture;
						textureUI->Texture = texture;
					}
				}
			}
		}
	}

	void CMaterial::loadDefaultTexture()
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		CTextureManager *textureManager = CTextureManager::getInstance();

		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader != NULL)
		{
			for (int i = 0, n = shader->getNumResource(); i < n; i++)
			{
				CShader::SResource *r = shader->getResouceInfo(i);

				CShader::SUniform *uniform = shader->getFSUniform(r->Name.c_str());
				if (uniform != NULL)
				{
					int textureSlot = (int)uniform->Value[0];

					ITexture* &texture = m_resourceTexture[textureSlot];

					// drop current holder
					if (texture != NULL)
					{
						texture->drop();
						texture = NULL;
					}

					// we will custom override texture first
					ITexture *overrideTexture = m_overrideTextures[r->Type];
					if (overrideTexture != NULL)
					{
						texture = overrideTexture;
					}
					else
					{
						if (r->Type == CShader::RTexture)
						{
							texture = textureManager->getTexture(r->Path.c_str());
						}
						else if (r->Type == CShader::RCubeTexture)
						{

						}
						else if (r->Type == CShader::RStaticCubeTexture)
						{

						}
						else if (r->Type == CShader::RShadowMapTexture)
						{

						}
					}

					// hold this texture
					if (texture != NULL)
						texture->grab();
				}
			}
		}
	}

	void CMaterial::addAffectMesh(IMeshBuffer *mesh)
	{
		if (mesh == NULL)
			return;

		for (int i = 0, n = (int)m_meshBuffers.size(); i < n; i++)
		{
			if (m_meshBuffers[i] == mesh)
				return;
		}

		m_meshBuffers.push_back(mesh);
		mesh->grab();

		mesh->getMaterial().MaterialInfo = this;
	}

	void CMaterial::removeAffectMesh(IMeshBuffer *mesh)
	{
		for (int i = 0, n = (int)m_meshBuffers.size(); i < n; i++)
		{
			if (m_meshBuffers[i] == mesh)
			{
				m_meshBuffers[i]->drop();
				m_meshBuffers.erase(m_meshBuffers.begin() + i);
				return;
			}
		}
	}

	void CMaterial::clearAllAffectMesh()
	{
		for (int i = 0, n = (int)m_meshBuffers.size(); i < n; i++)
		{
			m_meshBuffers[i]->drop();
		}
		m_meshBuffers.clear();
	}

	void CMaterial::initMaterial()
	{
		deleteAllParams();

		CShaderManager *shaderManager = CShaderManager::getInstance();
		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader != NULL)
		{
			// check pipeline
			m_deferred = shader->isDeferred();

			// add gui
			int numUI = shader->getNumUI();
			for (int i = 0; i < numUI; i++)
			{
				CShader::SUniformUI *ui = shader->getUniformUI(i);
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
	}

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

		for (int i = 0, n = (int)m_meshBuffers.size(); i < n; i++)
		{
			IMeshBuffer *meshBuffer = m_meshBuffers[i];
			video::SMaterial& material = meshBuffer->getMaterial();

			// apply texture
			updateTexture(material);
		}
	}

	// PRIVATE & PROTECTED

	void CMaterial::initDefaultValue()
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader != NULL)
		{
			int numVS = shader->getNumVS();
			for (int i = 0; i < numVS; i++)
			{
				CShader::SUniform* u = shader->getVSUniformID(i);
				if (u->Type == CShader::OBJECT_PARAM || u->Type == CShader::NODE_PARAM)
				{
					if (haveUniform(u->Name.c_str()) == false)
					{
						// need config default value from shader (first-time)
						SUniformValue* v = getUniform(u->Name.c_str());
						setDefaultValue(v, u);
					}
					else
					{
						// revert default value
						SUniformValue* v = getUniform(u->Name.c_str());
						if (v->ShaderDefaultValue == true)
							setDefaultValue(v, u);
					}
				}
			}

			int numFS = shader->getNumFS();
			for (int i = 0; i < numFS; i++)
			{
				CShader::SUniform* u = shader->getFSUniformID(i);
				if (u->Type == CShader::OBJECT_PARAM || u->Type == CShader::NODE_PARAM)
				{
					if (haveUniform(u->Name.c_str()) == false)
					{
						// need config default value from shader (first-time)
						SUniformValue* v = getUniform(u->Name.c_str());
						setDefaultValue(v, u);
					}
					else
					{
						// revert default value
						SUniformValue* v = getUniform(u->Name.c_str());
						if (v->ShaderDefaultValue == true)
							setDefaultValue(v, u);
					}
				}
			}
		}
	}

	CMaterial::SUniformValue* CMaterial::newUniform(const char *name, int floatSize)
	{
		SUniformValue *p = new SUniformValue();
		p->Name = name;
		p->FloatSize = floatSize;
		m_uniformParams.push_back(p);
		return p;
	}

	CMaterial::SUniformTexture* CMaterial::newUniformTexture(const char *name)
	{
		SUniformTexture *p = new SUniformTexture();
		p->Name = name;
		p->Texture = NULL;
		m_uniformTextures.push_back(p);
		return p;
	}

	void CMaterial::updateTexture(SMaterial& mat)
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();

		// property
		mat.ZBuffer = m_zBuffer;
		mat.ZWriteEnable = m_zWriteEnable;
		mat.BackfaceCulling = m_backfaceCulling;
		mat.FrontfaceCulling = m_frontfaceCulling;

		// apply filter
		mat.UseMipMaps = true;
		mat.setFlag(video::EMF_BILINEAR_FILTER, false);
		mat.setFlag(video::EMF_TRILINEAR_FILTER, false);

		if (getVideoDriver()->getDriverType() == video::EDT_OPENGLES)
			mat.setFlag(video::EMF_ANISOTROPIC_FILTER, true, 4);
		else
			mat.setFlag(video::EMF_ANISOTROPIC_FILTER, true, 8);

		// apply default shader material
		for (int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
		{
			if (m_resourceTexture[i] != NULL)
			{
				mat.setTexture(i, m_resourceTexture[i]);

				// todo
				// we clamp border, that dont make shadow map uv repeat
				if (i == m_shadowMapTextureSlot)
				{
					// turn off mipmap
					if (getVideoDriver()->getDriverType() == EDT_OPENGLES)
					{
						mat.TextureLayer[i].BilinearFilter = false;
						mat.TextureLayer[i].TrilinearFilter = false;
						mat.TextureLayer[i].AnisotropicFilter = 0;
					}

					mat.TextureLayer[i].TextureWrapU = video::ETC_CLAMP_TO_BORDER;
					mat.TextureLayer[i].TextureWrapV = video::ETC_CLAMP_TO_BORDER;
					mat.TextureLayer[i].BorderColor.set(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}
		}

		// apply config material
		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader != NULL)
		{
			mat.MaterialType = shaderManager->getShaderIDByName(shader->getName().c_str());

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
				CShader::SUniform *uniform = shader->getFSUniform(m_uniformTextures[i]->Name.c_str());
				if (uniform != NULL)
				{
					int textureSlot = (int)uniform->Value[0];
					if (textureSlot >= 0 && textureSlot < MATERIAL_MAX_TEXTURES)
					{
						mat.setTexture(textureSlot, m_uniformTextures[i]->Texture);
						m_textures[textureSlot] = m_uniformTextures[i]->Texture;
					}
				}
			}
		}
	}

	void CMaterial::updateShaderTexture()
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformTextures.size(); i < n; i++)
		{
			SUniformTexture *textureUI = m_uniformTextures[i];

			CShader::SUniform* uniform = shader->getFSUniform(textureUI->Name.c_str());
			if (uniform != NULL)
			{
				int textureSlot = (int)uniform->Value[0];
				if (textureSlot < MATERIAL_MAX_TEXTURES)
				{
					ITexture *texture = m_textures[textureSlot];
					if (texture)
					{
						textureUI->Path = texture->getName().getPath().c_str();
						textureUI->Texture = texture;
					}
				}
			}
		}
	}

	void CMaterial::updateShaderParams()
	{
		CShaderManager *shaderManager = CShaderManager::getInstance();
		CShader* shader = shaderManager->getShaderByPath(m_shaderPath.c_str());
		if (shader == NULL)
			return;

		for (int i = 0, n = (int)m_uniformParams.size(); i < n; i++)
		{
			SUniformValue *uniformValue = m_uniformParams[i];

			CShader::SUniform* uniform = shader->getFSUniform(uniformValue->Name.c_str());
			if (uniform == NULL)
				uniform = shader->getVSUniform(uniformValue->Name.c_str());

			if (uniform != NULL)
			{
				switch (uniform->Type)
				{
				case CShader::OBJECT_PARAM:
				{
					SVec4& v = m_owner->getShaderParams().getParam(uniform->ValueIndex);
					v.X = uniformValue->FloatValue[0];
					v.Y = uniformValue->FloatValue[1];
					v.Z = uniformValue->FloatValue[2];
					v.W = uniformValue->FloatValue[3];
					break;
				}
				case CShader::NODE_PARAM:
				{
					SVec4& v = m_shaderParams.getParam(uniform->ValueIndex);
					v.X = uniformValue->FloatValue[0];
					v.Y = uniformValue->FloatValue[1];
					v.Z = uniformValue->FloatValue[2];
					v.W = uniformValue->FloatValue[3];
					break;
				}
				/*
				case CShader::DEFAULT_VALUE:
				case CShader::CONFIG_VALUE:
					uniform->Value[0] = uniformValue->FloatValue[0];
					uniform->Value[1] = uniformValue->FloatValue[1];
					uniform->Value[2] = uniformValue->FloatValue[2];
					uniform->Value[3] = uniformValue->FloatValue[3];
					break;
				*/
				default:
				{
					char log[512];
					sprintf(log, "[CMaterial] - Warning!!! Fail to apply shader param type != OBJECT_PARAM");
					os::Printer::log(log);
					break;
				}
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
			if (ui->ControlType == CShader::UIColor)
			{
				// add color uniform
				newUniform(ui->Name.c_str(), ui->UniformInfo->FloatSize);
			}
			else if (ui->ControlType == CShader::UISlider)
			{
				// add float uniform
				newUniform(ui->Name.c_str(), ui->UniformInfo->FloatSize);
			}

			// apply default shader value
			setUniform4(ui->Name.c_str(), ui->UniformInfo->Value);
		}
	}

	void CMaterial::setDefaultValue(SUniformValue *v, CShader::SUniform* u)
	{
		memcpy(v->FloatValue, u->Value, sizeof(float) * 4);

		v->ShaderDefaultValue = true;
	}
}