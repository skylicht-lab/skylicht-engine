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

#pragma once

#include "Shader/CShaderParams.h"
#include "Shader/CShader.h"

#include "GameObject/CGameObject.h"

namespace Skylicht
{
	class CMaterial
	{
	public:
		struct SUniformTexture
		{
			std::string Name;
			std::string Path;
			ITexture* Texture;

			SUniformTexture()
			{
				Texture = NULL;
			}

			SUniformTexture* clone()
			{
				SUniformTexture *c = new SUniformTexture();
				c->Name = Name;
				c->Path = Path;
				c->Texture = Texture;
				return c;
			}
		};

		struct SUniformValue
		{
			std::string Name;
			float FloatValue[4];
			int FloatSize;
			bool ShaderDefaultValue;

			SUniformValue()
			{
				ShaderDefaultValue = false;
				FloatSize = 0;
				memset(FloatValue, 0, 4 * sizeof(float));
			}

			SUniformValue* clone()
			{
				SUniformValue *c = new SUniformValue();
				memcpy(c->FloatValue, FloatValue, sizeof(float) * 4);
				c->Name = Name;
				c->FloatSize = FloatSize;
				c->ShaderDefaultValue = ShaderDefaultValue;
				return c;
			}
		};

	private:
		CShaderParams m_shaderParams;

		std::string	m_materialName;
		std::string	m_shaderPath;

		std::vector<SUniformValue*> m_uniformParams;
		std::vector<SUniformTexture*> m_uniformTextures;

		std::vector<IMeshBuffer*> m_meshBuffers;

		ITexture *m_resourceTexture[MATERIAL_MAX_TEXTURES];
		ITexture *m_textures[MATERIAL_MAX_TEXTURES];
		ITexture *m_overrideTextures[CShader::RResourceCount];

		video::E_COMPARISON_FUNC m_zBuffer;
		bool m_zWriteEnable;
		bool m_backfaceCulling;
		bool m_frontfaceCulling;
		bool m_doubleSided;
		bool m_deferred;
		bool m_manualInitMaterial;
		int m_shadowMapTextureSlot;

		CGameObject* m_owner;

	public:
		CMaterial(const char *name, const char *shaderPath);

		virtual ~CMaterial();

		inline void setOwner(CGameObject *obj)
		{
			m_owner = obj;
		}

		CMaterial* clone(CGameObject *gameObject);

		void deleteAllParams();

		void setUniform(const char *name, float f);
		void setUniform2(const char *name, float *f);
		void setUniform3(const char *name, float *f);
		void setUniform4(const char *name, float *f);

		const char *getUniformTextureName(int slot);
		void setUniformTexture(const char *name, const char *path, bool loadTexture = true);
		void setUniformTexture(const char *name, const char *path, std::vector<std::string>& folder, bool loadTexture = true);
		void setUniformTexture(const char *name, ITexture *texture);

		SUniformValue* getUniform(const char *name);

		SUniformTexture* getUniformTexture(const char *name);

		bool haveUniform(const char *name);

		void initMaterial();

		void setTexture(ITexture** textures, int num);

		void setProperty(const std::string& name, const std::string& value);

		void loadDefaultTexture();

		void loadUniformTexture();

		void buildDoubleSidedMesh();

		void addAffectMesh(IMeshBuffer *mesh);

		void removeAffectMesh(IMeshBuffer *mesh);

		void clearAllAffectMesh();

		void setOverrideResource(ITexture *texture, CShader::EResourceType type)
		{
			m_overrideTextures[type] = texture;
		}

	public:

		void applyMaterial();

		void applyMaterial(SMaterial& mat);

	protected:
		void initDefaultValue();

		void updateTexture(SMaterial& mat);

		void updateShaderTexture();

		void updateShaderParams();

		void setDefaultValue(SUniformValue *v, CShader::SUniform* u);

		SUniformValue *newUniform(const char *name, int floatSize);

		SUniformTexture *newUniformTexture(const char *name);

		void addShaderUI(CShader::SUniformUI* ui);
	};

	typedef std::vector<CMaterial*> ArrayMaterial;
}