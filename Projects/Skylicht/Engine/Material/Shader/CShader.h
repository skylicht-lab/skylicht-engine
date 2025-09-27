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

	/**
	 * @brief Enum for all supported uniform types in shaders.
	 * See documentation page: @ref md__material_2_shader_2_r_e_a_d_m_e "Shader information structure".
	 */
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
		SHADOW_BIAS,
		WORLD_CAMERA_POSITION,
		LIGHT_COLOR,
		LIGHT_AMBIENT,
		WORLD_LIGHT_DIRECTION,
		POINT_LIGHT_COLOR,
		POINT_LIGHT_POSITION,
		POINT_LIGHT_ATTENUATION,
		SPOT_LIGHT_COLOR,
		SPOT_LIGHT_DIRECTION,
		SPOT_LIGHT_POSITION,
		SPOT_LIGHT_ATTENUATION,
		AREA_LIGHT_POSITION,
		AREA_LIGHT_DIR_X,
		AREA_LIGHT_DIR_Y,
		AREA_LIGHT_SIZE,
		AREA_LIGHT_COLOR,
		AREA_LIGHT_SHADOW_MATRIX,
		MATERIAL_PARAM,
		DEFAULT_VALUE,
		SHADER_VEC2,
		SHADER_VEC3,
		SHADER_VEC4,
		SH_CONST,
		CUSTOM_VALUE,
		TEXTURE_MIPMAP_COUNT,
		TEXTURE_WIDTH_HEIGHT,
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
		COLOR_INTENSITY,
		RENDER_TEXTURE_MATRIX,
		NUM_SHADER_TYPE,
	};

	/**
	 * @brief Structure describing a shader uniform.
	 * Holds name, type, values, binding info and platform specifics.
	 */
	struct SUniform
	{
		/// Uniform name (as in shader source)
		std::string Name;

		/// Uniform type (see EUniformType)
		EUniformType Type;

		/// Index for parameter arrays
		int ValueIndex;

		/// Default or set values
		float Value[16];

		/// Number of floats used
		int FloatSize;

		/// Array size(for bone matrices etc.)
		int ArraySize;

		/// True if uniform is matrix type
		bool IsMatrix;

		/// True if uniform is a normal map texture
		bool IsNormal;

		/// Total size in floats
		int SizeOfUniform;

		/// Supported on OpenGL
		bool OpenGL;

		/// Supported on DirectX
		/// Uniform texture doesn't need to be bound to a slot ID on the DirectX shader.
		/// Therefore, when the uniform is a texture, you should set DirectX=false in xml shader .
		bool DirectX;

		/// Internal shader variable ID
		int UniformShaderID;

		/// Minimum allowed value(for UI)
		float Min;

		/// Maximum allowed value(for UI)
		float Max;

		/**
		 * @brief Default constructor initializing values.
		 */
		SUniform()
		{
			OpenGL = true;
			DirectX = true;
			FloatSize = 1;
			ArraySize = 1;
			IsMatrix = false;
			IsNormal = false;

			ValueIndex = -1;
			memset(Value, 0, sizeof(float) * 16);
			UniformShaderID = -1;
			SizeOfUniform = 0;

			Type = NUM_SHADER_TYPE;
			Min = -FLT_MAX;
			Max = FLT_MAX;
		}
	};

	/**
	 * @brief Interface for shader callbacks, used to set constants in user-extended shaders.
	 */
	class SKYLICHT_API IShaderCallback
	{
	public:
		IShaderCallback()
		{

		}

		virtual ~IShaderCallback()
		{

		}

		/**
		 * @brief Called to set uniform constants for the shader.
		 * @param shader Pointer to the CShader instance.
		 * @param uniform Pointer to the uniform being set.
		 * @param matRender Material renderer.
		 * @param vertexShader True if setting vertex shader constants, false for pixel shader.
		 */
		virtual void OnSetConstants(CShader* shader, SUniform* uniform, IMaterialRenderer* matRender, bool vertexShader) = 0;
	};

	/// @brief Represents a programmable shader, including uniforms, UI, resources, instancing and platform-specific options
	/// @ingroup Materials
	/// 
	/// Handles parsing from XML, managing uniform parameters, callbacks, shadow/deferred settings, and integration with both OpenGL and DirectX.
	/// See documentation page: @ref md__material_2_shader_2_r_e_a_d_m_e "Shader information structure".
	class SKYLICHT_API CShader : public CBaseShaderCallback
	{
	public:
		/**
		 * @brief Enum for UI control types in material editor.
		 */
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

		/**
		 * @brief Enum for resource types used in shaders.
		 */
		enum EResourceType
		{
			Texture = 0,
			CubeTexture,
			ReflectionProbe,
			ShadowMap,
			TransformTexture,
			VertexPositionTexture,
			VertexNormalTexture,
			LastFrame,
			RTT0,
			RTT1,
			RTT2,
			RTT3,
			RTT4,
			RTT5,
			RTT6,
			RTT7,
			ResourceCount
		};

		/**
		 * @brief Structure for describing a UI element for a uniform.
		 * Used for editor integration and presentation.
		 */
		struct SUniformUI
		{
			/// Type of UI control
			EUIControlType ControlType;

			/// Uniform name
			std::string Name;

			/// Element names for vector types
			std::vector<std::string> ElementName;

			/// Auto - replace patterns for textures
			std::vector<std::string> AutoReplace;

			/// Minimum value for UI
			float Min;

			/// Maximum value for UI
			float Max;

			/// Pointer to uniform info
			SUniform* UniformInfo;

			/// Pointer to owning shader
			CShader* Shader;

			/// Parent UI group
			SUniformUI* Parent;

			/// Child UI elements (for groups)
			core::array<SUniformUI*> Childs;

			/**
			 * @brief Constructor.
			 * @param shader Pointer to owning shader.
			 */
			SUniformUI(CShader* shader)
			{
				UniformInfo = NULL;
				Shader = shader;
				Parent = NULL;
				Min = -FLT_MAX;
				Max = FLT_MAX;
				ControlType = UINone;
			}

			/**
			 * @brief Destructor. Cleans up child UI elements.
			 */
			~SUniformUI()
			{
				for (u32 i = 0, n = Childs.size(); i < n; i++)
				{
					delete Childs[i];
				}
			}
		};

		/**
		 * @brief Structure describing a shader resource (texture, cube map, etc.).
		 */
		struct SResource
		{
			std::string Name;
			std::string Path;
			EResourceType Type;
		};

		/**
		 * @brief Structure for storing shader source file names.
		 */
		struct SShader
		{
			/// Vertex shader file path
			std::string VertexShader;

			/// Fragment / pixel shader file path
			std::string FragmentShader;
		};

		/**
		 * @brief Structure for instancing shader information.
		 */
		struct SShaderInstancing
		{
			/// Vertex type for instancing
			video::E_VERTEX_TYPE VertexType;

			/// Name of instancing shader
			std::string ShaderName;

			/// Vertex type name for instancing
			/// "standard_color", "2texcoords_color", "standard_sg", "tangents_sg"
			std::string InstancingVertex;

			/// Custom instancing logic
			IShaderInstancing* Instancing;

			/// Pointer to instancing shader
			CShader* InstancingShader;

			SShaderInstancing()
			{
				VertexType = video::EVT_UNKNOWN;
				Instancing = NULL;
				InstancingShader = NULL;
			}
		};

		/**
		 * @brief Structure for mapping UI attributes to uniforms.
		 */
		struct SAttributeMapping
		{
			/// Name of the uniform
			std::string UniformName;

			/// Associated editor attribute name
			std::string AttributeName;
		};

	protected:
		/// Shader name
		std::string	m_name;

		/// Vertex shader uniforms
		core::array<SUniform> m_vsUniforms;

		/// Pixel / fragment shader uniforms
		core::array<SUniform> m_fsUniforms;

		/// UI elements
		core::array<SUniformUI*> m_ui;

		/// Shader texture resources
		core::array<SResource*> m_resources;

		/// UI to uniform mapping
		core::array<SAttributeMapping> m_attributeMapping;

		/// Shader callbacks
		std::vector<IShaderCallback*> m_callbacks;

		/// Dependent shader paths
		std::vector<std::string> m_dependents;

		/// Pointer to VS uniforms (m_vsUniforms)
		SUniform* m_listVSUniforms;

		/// Pointer to FS uniforms (m_fsUniforms)
		SUniform* m_listFSUniforms;

		/// Count of VS uniforms
		int m_numVSUniform;

		/// Count of FS uniforms
		int m_numFSUniform;

		/// GLSL shader source
		SShader	m_glsl;

		/// HLSL shader source
		SShader m_hlsl;

		/// Base material type(opaque, transparent, etc.)
		E_MATERIAL_TYPE	m_baseShader;

		/// Is shader for deferred rendering
		bool m_deferred;

		/// Is shader for skinning
		bool m_skinning;

		/// Is shader for shadow rendering
		bool m_shadow;

		/// Fallback software skinning shader name
		std::string m_softwareSkinningShaderName;

		/// Fallback software skinning shader
		CShader* m_softwareSkinningShader;

		/// Custom shadow depth shader name for directional light
		std::string m_shadowDepthShaderName;

		/// Custom shadow distance shader name for point light
		std::string m_shadowDistanceShaderName;

		/// Custom shadow depth shader
		CShader* m_shadowDepthShader;

		/// Custom shadow distance shader
		CShader* m_shadowDistanceShader;

		/// Instancing shader info by vertex type
		SShaderInstancing* m_shaderInstancing[video::EVT_UNKNOWN + 1];

		/// Source file path
		std::string m_source;
	public:
		/**
		 * @brief Constructor. Initializes internal structures and built-in callbacks.
		 */
		CShader();

		/**
		 * @brief Destructor. Releases resources, UI, callbacks and instancing shaders.
		 */
		virtual ~CShader();

		/**
		 * @brief Clear all internal data (resources, uniforms, UI, etc.).
		 */
		void clear();

		/**
		 * @brief Initialize shader from XML definition.
		 * @param xmlReader XML reader object pointing to shader definition.
		 * @param source Shader source file path.
		 * @param shaderFolder Folder containing shader files.
		 */
		void initShader(io::IXMLReader* xmlReader, const char* source, const char* shaderFolder);

	protected:
		/**
		 * @brief Parse uniform definitions from XML.
		 * @param xmlReader XML reader object.
		 */
		void parseUniform(io::IXMLReader* xmlReader);

		/**
		 * @brief Parse UI uniform definitions from XML.
		 * @param xmlReader XML reader object.
		 */
		void parseUniformUI(io::IXMLReader* xmlReader);

		/**
		 * @brief Parse resource definitions from XML.
		 * @param xmlReader XML reader object.
		 */
		void parseResources(io::IXMLReader* xmlReader);

		/**
		 * @brief Parse UI group hierarchy from XML.
		 * @param xmlReader XML reader object.
		 * @param parent Parent UI element (or NULL for root).
		 */
		void parseUI(io::IXMLReader* xmlReader, SUniformUI* parent);

	public:
		/**
		 * @brief Get the name of the shader.
		 * @return Reference to shader name string.
		 */
		inline const std::string& getName()
		{
			return m_name;
		}

		/**
		 * @brief Get the source file path of the shader.
		 * @return Reference to source path string.
		 */
		inline const std::string& getSource()
		{
			return m_source;
		}

		/**
		 * @brief Get the name of the software skinning fallback shader.
		 * @return Reference to shader name string.
		 */
		inline const std::string& getSoftwareSkinningName()
		{
			return m_softwareSkinningShaderName;
		}

		/**
		 * @brief Returns true if the shader supports skinning.
		 */
		inline bool isSkinning()
		{
			return m_skinning;
		}

		/**
		 * @brief Returns true if a custom shadow depth write shader is defined.
		 */
		inline bool isCustomShadowDepthWrite()
		{
			return !m_shadowDepthShaderName.empty();
		}

		/**
		 * @brief Returns true if a custom shadow distance write shader is defined.
		 */
		inline bool isCustomShadowDistanceWrite()
		{
			return !m_shadowDistanceShaderName.empty();
		}

		/**
		 * @brief Get the software skinning fallback shader.
		 * @return Pointer to CShader instance.
		 */
		CShader* getSoftwareSkinningShader();

		/**
		 * @brief Get the instancing shader for a specific vertex type.
		 * @param vtxType Vertex type.
		 * @return Pointer to instancing shader.
		 */
		CShader* getInstancingShader(video::E_VERTEX_TYPE vtxType);

		/**
		 * @brief Get the vertex type name for instancing for a specific vertex type.
		 * @param vtxType Vertex type.
		 * @return Name of the instancing vertex entry point.
		 */
		std::string getInstancingVertex(video::E_VERTEX_TYPE vtxType);

		/**
		 * @brief Returns true if instancing is supported for the given vertex type.
		 * @param vtxType Vertex type.
		 * @return True if supported, false otherwise.
		 */
		bool isSupportInstancing(video::E_VERTEX_TYPE vtxType);

		/**
		 * @brief Set a custom instancing logic for a specific vertex type.
		 * @param vtxType Vertex type.
		 * @param instancing Pointer to IShaderInstancing implementation.
		 */
		void setInstancing(video::E_VERTEX_TYPE vtxType, IShaderInstancing* instancing);

		/**
		 * @brief Get the custom instancing logic for a specific vertex type.
		 * @param vtxType Vertex type.
		 * @return Pointer to IShaderInstancing.
		 */
		IShaderInstancing* getInstancing(video::E_VERTEX_TYPE vtxType);

		/**
		 * @brief Set the custom shadow depth write shader by name.
		 * @param name Shader name.
		 */
		void setShadowDepthWriteShader(const char* name);

		/**
		 * @brief Set the custom shadow distance write shader by name.
		 * @param name Shader name.
		 */
		void setShadowDistanceWriteShader(const char* name);

		/**
		 * @brief Get the custom shadow depth write shader.
		 * @return Pointer to CShader instance.
		 */
		CShader* getShadowDepthWriteShader();

		/**
		 * @brief Get the custom shadow distance write shader.
		 * @return Pointer to CShader instance.
		 */
		CShader* getShadowDistanceWriteShader();

		/**
		 * @brief Get the number of attribute mappings for editor integration.
		 * @return Number of attribute mappings.
		 */
		inline int getAttributeMappingCount()
		{
			return (int)m_attributeMapping.size();
		}

		/**
		 * @brief Get an attribute mapping by index.
		 * @param id Index of the mapping.
		 * @return Reference to SAttributeMapping.
		 */
		inline SAttributeMapping& getAttributeMapping(int id)
		{
			return m_attributeMapping[id];
		}

		/**
		 * @brief Get the number of texture resources bind in shader.
		 * @return Number of resources.
		 */
		inline int getNumResource()
		{
			return (int)m_resources.size();
		}

		/**
		 * @brief Get the number of vertex shader uniforms.
		 * @return Number of VS uniforms.
		 */
		inline int getNumVS()
		{
			return (int)m_vsUniforms.size();
		}

		/**
		 * @brief Get the number of fragment shader uniforms.
		 * @return Number of FS uniforms.
		 */
		inline int getNumFS()
		{
			return (int)m_fsUniforms.size();
		}

		/**
		 * @brief Get a shader resource info by index.
		 * @param id Index.
		 * @return Pointer to SResource.
		 */
		inline SResource* getResouceInfo(int id)
		{
			return m_resources[id];
		}

		/**
		 * @brief Get a vertex shader uniform by index.
		 * @param id Index.
		 * @return Pointer to SUniform.
		 */
		inline SUniform* getVSUniformID(int id)
		{
			return &m_listVSUniforms[id];
		}

		/**
		 * @brief Get a fragment shader uniform by index.
		 * @param id Index.
		 * @return Pointer to SUniform.
		 */
		inline SUniform* getFSUniformID(int id)
		{
			return &m_listFSUniforms[id];
		}

		/**
		 * @brief Get the number of UI elements.
		 * @return Number of UI elements.
		 */
		inline int getNumUI()
		{
			return (int)m_ui.size();
		}

		/**
		 * @brief Get a UI element by index.
		 * @param id Index.
		 * @return Pointer to SUniformUI.
		 */
		inline SUniformUI* getUniformUI(int id)
		{
			return m_ui[id];
		}

		/**
		 * @brief Get the list of dependent shader paths.
		 * @return Reference to vector of dependent shader paths.
		 */
		inline std::vector<std::string>& getDependents()
		{
			return m_dependents;
		}

		/**
		 * @brief Get a UI element by uniform name.
		 * @param name Uniform name.
		 * @return Pointer to SUniformUI.
		 */
		SUniformUI* getUniformUIByName(const char* name);

		/**
		 * @brief Get a UI element by uniform name within a UI group.
		 * @param name Uniform name.
		 * @param group UI group parent.
		 * @return Pointer to SUniformUI.
		 */
		SUniformUI* getUniformUIByName(const char* name, SUniformUI* group);

		/**
		 * @brief Get a vertex shader uniform by name.
		 * @param name Uniform name.
		 * @return Pointer to SUniform.
		 */
		SUniform* getVSUniform(const char* name);

		/**
		 * @brief Get a fragment shader uniform by name.
		 * @param name Uniform name.
		 * @return Pointer to SUniform.
		 */
		SUniform* getFSUniform(const char* name);

		/**
		 * @brief Returns true if this shader is used for deferred rendering.
		 */
		inline bool isDeferred()
		{
			return m_deferred;
		}

		/**
		 * @brief Returns true if this shader is an opaque base shader.
		 */
		inline bool isOpaque()
		{
			return m_baseShader == EMT_SOLID;
		}

		/**
		 * @brief Get the base material type of this shader.
		 * @return E_MATERIAL_TYPE value.
		 */
		inline E_MATERIAL_TYPE getBaseMaterial()
		{
			return m_baseShader;
		}

	public:

		/**
		 * @brief Callback to set shader constants (uniforms) for rendering.
		 * @param services Renderer services.
		 * @param userData User data.
		 * @param updateTransform True to update transforms.
		 */
		virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData, bool updateTransform);

		/**
		 * @brief Add a custom callback to the shader for setting constants.
		 * @tparam T The callback class, must inherit IShaderCallback.
		 * @return Pointer to the created callback.
		 */
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

		/**
		 * @brief Get a custom callback of a specific type.
		 * @tparam T The callback class type.
		 * @return Pointer to the callback instance, or NULL if not found.
		 */
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

		/**
		 * @brief Build the shader by compiling vertex and fragment sources and registering with the engine.
		 */
		void buildShader();

		/**
		 * @brief Build UI uniform bindings for editor integration.
		 */
		void buildUIUniform();

		/**
		 * @brief Returns true if this shader renders depth for shadows.
		 */
		inline bool isDrawDepthShadow()
		{
			return m_shadow;
		}

	protected:
		/**
		 * @brief Get the base shader type by its name string (SOLID, TRANSPARENT, etc.).
		 * @param name Name string.
		 * @return E_MATERIAL_TYPE value.
		 */
		E_MATERIAL_TYPE getBaseShaderByName(const char* name);

		/**
		 * @brief Get the uniform type enum from its name string.
		 * @param name Uniform name.
		 * @return EUniformType value.
		 */
		EUniformType getUniformType(const char* name);

		/**
		 * @brief Get the full path to the vertex shader source file (GLSL/HLSL).
		 * @return Vertex shader source file path.
		 */
		std::string getVSShaderFileName();

		/**
		 * @brief Get the full path to the fragment/pixel shader source file (GLSL/HLSL).
		 * @return Fragment shader source file path.
		 */
		std::string getFSShaderFileName();

		/**
		 * @brief Recursively build UI uniform bindings for a UI element.
		 * @param ui Pointer to SUniformUI element.
		 */
		void buildUIUniform(SUniformUI* ui);

		/**
		 * @brief Check if a uniform is available on the current graphics API.
		 * @param uniform Reference to SUniform.
		 * @return True if available, false otherwise.
		 */
		bool isUniformAvaiable(SUniform& uniform);

		/**
		 * @brief Try to set a uniform value for rendering.
		 * @param uniform Reference to SUniform.
		 * @param matRender Material renderer.
		 * @param vertexShader True for vertex shader, false for pixel shader.
		 * @param updateTransform True to update transforms. This is because the object's transform is sometimes unchanged, meaning there's no need to update it.
		 * @return True if handled as built-in, false if should call plugin callback.
		 */
		bool setUniform(SUniform& uniform, IMaterialRenderer* matRender, bool vertexShader, bool updateTransform);

		/**
		 * @brief Delete all UI elements.
		 */
		void deleteAllUI();

		/**
		 * @brief Delete all shader resources.
		 */
		void deleteAllResource();
	};

}