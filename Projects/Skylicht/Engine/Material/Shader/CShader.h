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

#include "CBaseShaderCallback.h"
#include "Instancing/IShaderInstancing.h"

namespace Skylicht
{
	class CShader;

	enum EUniformType
	{
		VIEW_PROJECTION,
		WORLD_VIEW_PROJECTION,
		VIEW,
		WORLD,
		WORLD_INVERSE,
		WORLD_INVERSE_TRANSPOSE,
		WORLD_TRANSPOSE,
		BONE_MATRIX,
		BONE_COUNT,
		SHADOW_MAP_MATRIX,
		SHADOW_MAP_DISTANCE,
		CAMERA_POSITION,
		WORLD_CAMERA_POSITION,
		LIGHT_COLOR,
		LIGHT_AMBIENT,
		LIGHT_DIRECTION,
		WORLD_LIGHT_DIRECTION,
		POINT_LIGHT_COLOR,
		POINT_LIGHT_POSITION,
		POINT_LIGHT_ATTENUATION,
		SPOT_LIGHT_COLOR,
		SPOT_LIGHT_DIRECTION,
		SPOT_LIGHT_POSITION,
		SPOT_LIGHT_ATTENUATION,
		MATERIAL_PARAM,
		DEFAULT_VALUE,
		SHADER_VEC2,
		SHADER_VEC3,
		SHADER_VEC4,
		SH_CONST,
		CUSTOM_VALUE,
		BPCEM_MIN,
		BPCEM_MAX,
		BPCEM_POS,
		TEXTURE_MIPMAP_COUNT,
		TEXTURE_WIDTH_HEIGHT,
		FOG_PARAMS,
		SSAO_KERNEL,
		DEFERRED_VIEW,
		DEFERRED_PROJECTION,
		DEFERRED_VIEW_PROJECTION,
		TIME_STEP,
		PARTICLE_VIEW_UP,
		PARTICLE_VIEW_LOOK,
		PARTICLE_ORIENTATION_UP,
		PARTICLE_ORIENTATION_NORMAL,
		LIGHTMAP_INDEX,
		TIME,
		NUM_SHADER_TYPE,
	};

	struct SUniform
	{
		std::string Name;
		EUniformType Type;

		int	ValueIndex;
		float Value[16];
		int FloatSize;
		int ArraySize;
		bool IsMatrix;
		bool IsNormal;

		int SizeOfUniform;

		bool OpenGL;
		bool DirectX;

		int UniformShaderID;

		float Min;
		float Max;

		SUniform()
		{
			OpenGL = true;
			DirectX = true;
			FloatSize = 1;
			ArraySize = 1;
			IsMatrix = false;
			IsNormal = false;

			ValueIndex = 0;
			memset(Value, 0, sizeof(float) * 16);
			UniformShaderID = -1;
			SizeOfUniform = 0;

			Min = -FLT_MAX;
			Max = FLT_MAX;
		}
	};

	class SKYLICHT_API IShaderCallback
	{
	public:
		IShaderCallback()
		{

		}

		virtual ~IShaderCallback()
		{

		}

		virtual void OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader) = 0;
	};

	class SKYLICHT_API CShader : public CBaseShaderCallback
	{
	public:
		enum EUIControlType
		{
			UITexture = 0,
			UIColor,
			UIFloat,
			UIFloat2,
			UIFloat3,
			UIFloat4,
			UIGroup,
			UINone,
			UICount
		};

		enum EResourceType
		{
			Texture = 0,
			CubeTexture,
			ReflectionProbe,
			ShadowMap,
			TransformTexture,
			VertexPositionTexture,
			VertexNormalTexture,
			ResourceCount
		};

		struct SUniformUI
		{
			EUIControlType ControlType;
			std::string Name;
			std::vector<std::string> ElementName;
			std::vector<std::string> AutoReplace;
			float Min;
			float Max;

			SUniform* UniformInfo;
			CShader* Shader;

			SUniformUI* Parent;
			core::array<SUniformUI*> Childs;

			SUniformUI(CShader* shader)
			{
				UniformInfo = NULL;
				Shader = shader;
				Parent = NULL;
				Min = -FLT_MAX;
				Max = FLT_MAX;
				ControlType = UINone;
			}

			~SUniformUI()
			{
				for (u32 i = 0, n = Childs.size(); i < n; i++)
				{
					delete Childs[i];
				}
			}
		};

		struct SResource
		{
			std::string	Name;
			std::string	Path;
			EResourceType Type;
		};

		struct SShader
		{
			std::string	VertexShader;
			std::string FragmentShader;
		};

		struct SAttributeMapping
		{
			std::string UniformName;
			std::string AttributeName;
		};

	protected:
		std::string	m_name;
		std::string	m_shaderPath;

		core::array<SUniform> m_vsUniforms;
		core::array<SUniform> m_fsUniforms;
		core::array<SUniformUI*> m_ui;
		core::array<SResource*> m_resources;

		core::array<SAttributeMapping> m_attributeMapping;

		std::vector<IShaderCallback*> m_callbacks;

		std::vector<std::string> m_dependents;

		SUniform* m_listVSUniforms;
		SUniform* m_listFSUniforms;
		int m_numVSUniform;
		int m_numFSUniform;

		SShader	m_glsl;
		SShader m_hlsl;

		E_MATERIAL_TYPE	m_baseShader;

		bool m_deferred;
		bool m_skinning;
		bool m_shadow;

		// fallback for hardware skinning
		std::string m_softwareSkinningShaderName;
		CShader* m_softwareSkinningShader;

		IShaderInstancing* m_instancing;

		video::E_VERTEX_TYPE m_vertexType;

		std::string m_instancingShaderName;
		std::string m_shadowDepthShaderName;
		std::string m_shadowDistanceShaderName;

		CShader* m_instancingShader;
		CShader* m_shadowDepthShader;
		CShader* m_shadowDistanceShader;

	public:

		CShader();

		virtual ~CShader();

		void initShader(io::IXMLReader* xmlReader, const char* shaderFolder);

		void parseUniform(io::IXMLReader* xmlReader);

		void parseUniformUI(io::IXMLReader* xmlReader);

		void parseResources(io::IXMLReader* xmlReader);

		void parseUI(io::IXMLReader* xmlReader, SUniformUI* parent);

		inline const std::string& getName()
		{
			return m_name;
		}

		inline const std::string& getShaderPath()
		{
			return m_shaderPath;
		}

		inline const std::string& getSoftwareSkinningName()
		{
			return m_softwareSkinningShaderName;
		}

		inline bool isSkinning()
		{
			return m_skinning;
		}

		inline bool isSupportInstancing()
		{
			return !m_instancingShaderName.empty();
		}

		inline bool isCustomShadowDepthWrite()
		{
			return !m_shadowDepthShaderName.empty();
		}

		inline bool isCustomShadowDistanceWrite()
		{
			return !m_shadowDistanceShaderName.empty();
		}

		CShader* getSoftwareSkinningShader();

		CShader* getInstancingShader();

		CShader* getShadowDepthWriteShader();

		CShader* getShadowDistanceWriteShader();

		inline video::E_VERTEX_TYPE getVertexType()
		{
			return m_vertexType;
		}

		inline int getAttributeMappingCount()
		{
			return (int)m_attributeMapping.size();
		}

		inline SAttributeMapping& getAttributeMapping(int id)
		{
			return m_attributeMapping[id];
		}

		inline void setShaderPath(const char* path)
		{
			m_shaderPath = path;
		}

		inline int getNumResource()
		{
			return (int)m_resources.size();
		}

		inline int getNumVS()
		{
			return (int)m_vsUniforms.size();
		}

		inline int getNumFS()
		{
			return (int)m_fsUniforms.size();
		}

		inline SResource* getResouceInfo(int id)
		{
			return m_resources[id];
		}

		inline SUniform* getVSUniformID(int id)
		{
			return &m_listVSUniforms[id];
		}

		inline SUniform* getFSUniformID(int id)
		{
			return &m_listFSUniforms[id];
		}

		inline int getNumUI()
		{
			return (int)m_ui.size();
		}

		inline SUniformUI* getUniformUI(int id)
		{
			return m_ui[id];
		}

		inline std::vector<std::string>& getDependents()
		{
			return m_dependents;
		}

		SUniformUI* getUniformUIByName(const char* name);

		SUniformUI* getUniformUIByName(const char* name, SUniformUI* group);

		SUniform* getVSUniform(const char* name);

		SUniform* getFSUniform(const char* name);

		inline bool isDeferred()
		{
			return m_deferred;
		}

		inline bool isOpaque()
		{
			return m_baseShader == EMT_SOLID;
		}

		inline E_MATERIAL_TYPE getBaseMaterial()
		{
			return m_baseShader;
		}

	public:

		// shader callback
		virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData, bool updateTransform);

		template<class T>
		IShaderCallback* addCallback()
		{
			T* newCB = new T();
			IShaderCallback* shaderCallback = dynamic_cast<IShaderCallback*>(newCB);
			if (shaderCallback == NULL)
			{
				char exceptionInfo[512];
				sprintf(exceptionInfo, "CShader::addCallback %s must inherit IShaderCallback", typeid(T).name());
				os::Printer::log(exceptionInfo);

				delete newCB;
				return NULL;
			}

			m_callbacks.push_back(newCB);
			return newCB;
		}

		template<class T>
		T* getCallback()
		{
			for (IShaderCallback* cb : m_callbacks)
			{
				if (typeid(T) == typeid(*cb))
				{
					return (T*)cb;
				}
			}

			return NULL;
		}

		void buildShader();

		void buildUIUniform();

		void setInstancing(IShaderInstancing* instancing)
		{
			if (m_instancing)
				delete m_instancing;

			m_instancing = instancing;
		}

		inline IShaderInstancing* getInstancing()
		{
			return m_instancing;
		}

		inline bool isDrawDepthShadow()
		{
			return m_shadow;
		}

	protected:

		E_MATERIAL_TYPE getBaseShaderByName(const char* name);

		EUniformType getUniformType(const char* name);

		std::string getVSShaderFileName();

		std::string getFSShaderFileName();

		void buildUIUniform(SUniformUI* ui);

		bool isUniformAvaiable(SUniform& uniform);

		bool setUniform(SUniform& uniform, IMaterialRenderer* matRender, bool vertexShader, bool updateTransform);

		void deleteAllUI();

		void deleteAllResource();
	};

}