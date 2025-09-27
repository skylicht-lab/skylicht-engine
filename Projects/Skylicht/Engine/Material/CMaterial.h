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
	/// @brief The object class describes material information
	/// such as which shader it's associated with, which texture it uses, and what its color parameters are.
	/// @ingroup Materials
	/// 
	/// **Skylicht-Editor**
	/// 
	/// You can create materials in two ways
	/// 
	/// **Method 1:** Select the model file (.fbx, .dae) and click the "Export Material" button
	/// @image html Material/export-material.jpg
	/// 
	/// **Method 2:** Create it manually by choosing the menu Asset/Create/Material.
	/// @image html Material/material-asset.jpg
	/// 
	/// **Adjusting materials:**
	/// 
	/// The Material Property window allows you to add multiple materials within a .mat file. It also includes functions like delete/rename.
	/// 
	/// @image html Material/material-property-window.jpg "Material property"
	/// <br>
	/// @image html Material/material-tool.jpg "Delete/Rename Material"
	/// 
	/// **Next**, drag the shader .xml file into the Shader Field to select the material's shader. 
	/// 
	/// From there, you can then drag texture files into the various uniform textures, or adjust the parameters defined within the shader .xml.
	/// 
	/// @image html Material/material.jpg
	/// 
	/// Materials are essential for the CRenderMesh component to display the model's appearance on the Skylicht-Engine
	/// 
	/// @image html RenderMesh/render-mesh-property.jpg width=1200px
	/// 
	/// 
	/// You use CTextureManager to support loading .mat material files.
	/// @code
	/// std::vector<std::string> textureFolders;
	/// ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
	/// 	"SampleModels/BlendShape/Cat.mat",
	/// 	true,
	/// 	textureFolders
	/// );
	/// 
	/// for (CMaterial* mat: materials)
	/// {
	/// 	printf("%s\n", mat->getName());
	/// }
	/// @endcode
	class SKYLICHT_API CMaterial : public IReferenceCounted
	{
	public:

		/**
		 * @brief Structure for a uniform texture parameter in the material.
		 *
		 * Holds information about the texture, its name, path, slot, wrapping modes, and filtering options.
		 */
		struct SUniformTexture
		{
			/// Name of the uniform texture
			std::string Name;

			/// Path to the texture file
			std::string Path;

			/// Pointer to loaded texture resource
			ITexture* Texture;

			/// Texture slot index in the shader
			int TextureSlot;

			/// Wrapping mode in U direction
			// @see irr::video::E_TEXTURE_CLAMP
			int WrapU;

			/// Wrapping mode in V direction
			// @see irr::video::E_TEXTURE_CLAMP
			int WrapV;

			/// Anisotropic filtering level
			int Anisotropic;

			/// Trilinear filtering enabled
			bool Trilinear;

			/// Bilinear filtering enabled
			bool Bilinear;

			/**
			 * @brief Default constructor initializing default values.
			 */
			SUniformTexture()
			{
				Texture = NULL;
				TextureSlot = -1;
				WrapU = 0; // Repeat
				WrapV = 0; // Repeat
				Anisotropic = 2;
				Trilinear = false;
				Bilinear = false;
			}

			/**
			 * @brief Creates a deep copy of the uniform texture.
			 * @return Pointer to a new SUniformTexture instance.
			 */
			SUniformTexture* clone()
			{
				SUniformTexture* c = new SUniformTexture();
				c->Name = Name;
				c->Path = Path;
				c->Texture = Texture;
				c->TextureSlot = TextureSlot;
				c->WrapU = WrapU;
				c->WrapV = WrapV;
				c->Anisotropic = Anisotropic;
				c->Trilinear = Trilinear;
				c->Bilinear = Bilinear;
				return c;
			}
		};

		/**
		 * @brief Structure for a uniform value parameter in the material.
		 *
		 * Stores uniform name, type, value data, and binding information.
		 */
		struct SUniformValue
		{
			// Name of the uniform
			std::string Name;

			// Value data (up to 4 floats)
			float FloatValue[4];

			// Number of floats used
			int FloatSize;

			// True if value comes from shader default
			bool ShaderDefaultValue;

			// Shader uniform type
			EUniformType Type;

			// Shader parameter index
			int ValueIndex;

			/**
			 * @brief Default constructor initializing default values.
			 */
			SUniformValue()
			{
				ShaderDefaultValue = false;
				FloatSize = 0;
				Type = NUM_SHADER_TYPE;
				ValueIndex = -1;
				memset(FloatValue, 0, 4 * sizeof(float));
			}

			/**
			 * @brief Creates a deep copy of the uniform value.
			 * @return Pointer to a new SUniformValue instance.
			 */
			SUniformValue* clone()
			{
				SUniformValue* c = new SUniformValue();
				memcpy(c->FloatValue, FloatValue, sizeof(float) * 4);
				c->Name = Name;
				c->FloatSize = FloatSize;
				c->ShaderDefaultValue = ShaderDefaultValue;
				c->Type = Type;
				c->ValueIndex = ValueIndex;
				return c;
			}
		};

		/**
		 * @brief Structure for storing extra parameters, used when switching shaders.
		 *
		 * Contains shader path, additional uniform values and textures.
		 */
		struct SExtraParams
		{
			/// Associated shader path
			std::string ShaderPath;

			/// Extra uniform values
			std::vector<SUniformValue*> UniformParams;

			/// Extra uniform textures
			std::vector<SUniformTexture*> UniformTextures;
		};

	private:
		/// Internal shader parameter values
		/// This will copy value from m_uniformParams (see function updateShaderParams)
		CShaderParams m_shaderParams;

		/// Material package name
		std::string m_package;

		/// Material name
		std::string m_materialName;

		/// Path to the shader
		std::string m_shaderPath;

		/// Path to the material (.mat) file
		std::string m_materialPath;

		/// Uniform value parameters
		std::vector<SUniformValue*> m_uniformParams;

		/// Uniform texture parameters
		std::vector<SUniformTexture*> m_uniformTextures;

		/// Extra parameters for shader switching
		std::vector<SExtraParams*> m_extras;

		/// Default textures from shader resources
		ITexture* m_resourceTexture[MATERIAL_MAX_TEXTURES];

		/// Textures assigned to slots
		ITexture* m_textures[MATERIAL_MAX_TEXTURES];

		/// Overridden textures by resource type
		ITexture* m_overrideTextures[CShader::ResourceCount];

		/// Z-buffer comparison function
		video::E_COMPARISON_FUNC m_zBuffer;

		/// Enable writing to Z-buffer
		bool m_zWriteEnable;

		/// Enable backface culling
		bool m_backfaceCulling;

		/// Enable frontface culling
		bool m_frontfaceCulling;

		/// Render both sides of polygons
		bool m_doubleSided;

		/// Is material used in deferred rendering
		bool m_deferred;

		/// Can this material cast shadows
		bool m_castShadow;

		/// Manual material initialization flag
		bool m_manualInitMaterial;

		/// Pointer to associated shader
		CShader* m_shader;

	public:
		/**
		 * @brief Constructs a material with the given name and shader path.
		 * @param name Name of the material.
		 * @param shaderPath Path to the shader file.
		 */
		CMaterial(const char* name, const char* shaderPath);

		/**
		 * @brief Destructor. Releases textures, deletes all parameters and cleans up resources.
		 */
		virtual ~CMaterial();

		/**
		 * @brief Get the shader associated with this material.
		 * @return Pointer to the shader.
		 */
		inline CShader* getShader()
		{
			return m_shader;
		}

		/**
		 * @brief Rename the material.
		 * @param name New name for the material.
		 */
		void rename(const char* name)
		{
			m_materialName = name;
		}

		/**
		 * @brief Get the name of the material.
		 * @return Material name string.
		 */
		inline const char* getName()
		{
			return m_materialName.c_str();
		}

		/**
		 * @brief Get the package name of the material.
		 * @return Package name string.
		 * @see CMaterialManager
		 */
		inline const char* getPackage()
		{
			return m_package.c_str();
		}

		/**
		 * @brief Set the package name for the material.
		 * @param package Package name string.
		 * @see CMaterialManager
		 */
		inline void setPackage(const char* package)
		{
			m_package = package;
		}

		/**
		 * @brief Get the shader path of the material.
		 * @return Shader path string.
		 */
		inline const char* getShaderPath()
		{
			return m_shaderPath.c_str();
		}

		/**
		 * @brief Get the material file path.
		 * @return Material file path string.
		 */
		inline const char* getMaterialPath()
		{
			return m_materialPath.c_str();
		}

		/**
		 * @brief Set the material file path.
		 * @param path New material file path string.
		 */
		inline void setMaterialPath(const char* path)
		{
			m_materialPath = path;
		}

		/**
		 * @brief Returns true if the material is used for deferred rendering.
		 * @return True if deferred, false otherwise.
		 */
		inline bool isDeferred()
		{
			return m_deferred;
		}

		/**
		 * @brief Set manual initialization flag for textures.
		 * @param b True to enable manual initialization.
		 */
		inline void setManualInitTexture(bool b)
		{
			m_manualInitMaterial = b;
		}
		/**
		 * @brief Create a deep clone of this material.
		 * @return Pointer to the cloned material.
		 */
		CMaterial* clone();

		/**
		 * @brief Copy all parameters and resources to another material.
		 * @param mat Target material to copy parameters to.
		 */
		void copyTo(CMaterial* mat);

		/**
		 * @brief Copy only uniform parameters to another material.
		 * @param mat Target material to copy parameters to.
		 */
		void copyParamsTo(CMaterial* mat);

		/**
		 * @brief Delete all uniform parameters and textures.
		 */
		void deleteAllParams();

		/**
		 * @brief Delete all extra parameters (used for shader switching).
		 */
		void deleteExtraParams();

		/**
		 * @brief Set a float value for a named uniform.
		 * @param name Name of the uniform.
		 * @param f Value to set.
		 */
		void setUniform(const char* name, float f);

		/**
		 * @brief Set a vec2 value for a named uniform.
		 * @param name Name of the uniform.
		 * @param f Pointer to array of 2 floats.
		 */
		void setUniform2(const char* name, float* f);

		/**
		 * @brief Set a vec3 value for a named uniform.
		 * @param name Name of the uniform.
		 * @param f Pointer to array of 3 floats.
		 */
		void setUniform3(const char* name, float* f);

		/**
		 * @brief Set a vec4 value for a named uniform.
		 * @param name Name of the uniform.
		 * @param f Pointer to array of 4 floats.
		 */
		void setUniform4(const char* name, float* f);

		/**
		 * @brief Set a vec4 value for a named uniform from an SColor.
		 * @param name Name of the uniform.
		 * @param color Color value to set.
		 */
		void setUniform4(const char* name, const SColor& color);

		/**
		 * @brief Get the name of a uniform texture by slot index.
		 * @param slot Texture slot index.
		 * @return Name string, or NULL if not found.
		 */
		const char* getUniformTextureName(int slot);

		/**
		 * @brief Set a uniform texture by name and path, optionally loading the texture.
		 * @param name Uniform texture name.
		 * @param path Path to texture file.
		 * @param loadTexture True to load the texture.
		 */
		void setUniformTexture(const char* name, const char* path, bool loadTexture = true);

		/**
		 * @brief Set a uniform texture by name and path, searching in specified folders.
		 * @param name Uniform texture name.
		 * @param path Path to texture file.
		 * @param folder List of folders to search for the texture.
		 * @param loadTexture True to load the texture.
		 */
		void setUniformTexture(const char* name, const char* path, std::vector<std::string>& folder, bool loadTexture = true);

		/**
		 * @brief Set a uniform texture by name, using an ITexture pointer.
		 * @param name Uniform texture name.
		 * @param texture Pointer to texture.
		 */
		void setUniformTexture(const char* name, ITexture* texture);

		/**
		 * @brief Set the wrapping mode in U direction for a uniform texture.
		 * @param name Uniform texture name.
		 * @param wrapU Wrapping mode value. @see irr::video::E_TEXTURE_CLAMP
		 */
		void setUniformTextureWrapU(const char* name, int wrapU);

		/**
		 * @brief Set the wrapping mode in V direction for a uniform texture.
		 * @param name Uniform texture name.
		 * @param wrapV Wrapping mode value. @see irr::video::E_TEXTURE_CLAMP
		 */
		void setUniformTextureWrapV(const char* name, int wrapV);

		/**
		 * @brief Create a new extra parameter block for a specific shader path.
		 * @param shaderPath Shader path string.
		 * @return Pointer to new SExtraParams instance.
		 */
		SExtraParams* newExtra(const char* shaderPath);

		/**
		 * @brief Set the path for an extra uniform texture.
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform texture name.
		 * @param path Path to texture.
		 */
		void setExtraUniformTexture(SExtraParams* e, const char* name, const char* path);

		/**
		 * @brief Set a value for an extra uniform.
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform name.
		 * @param f Pointer to value array.
		 * @param floatSize Number of floats.
		 */
		void setExtraUniform(SExtraParams* e, const char* name, float* f, int floatSize);

		/**
		 * @brief Set wrapping mode U for extra uniform texture.
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform texture name.
		 * @param wrapU Wrapping mode value. @see irr::video::E_TEXTURE_CLAMP
		 */
		void setExtraUniformTextureWrapU(SExtraParams* e, const char* name, int wrapU);

		/**
		 * @brief Set wrapping mode V for extra uniform texture.
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform texture name.
		 * @param wrapV Wrapping mode value. @see irr::video::E_TEXTURE_CLAMP
		 */
		void setExtraUniformTextureWrapV(SExtraParams* e, const char* name, int wrapV);

		/**
		 * @brief Get a reference to the uniform value parameters vector.
		 * @return Reference to vector of SUniformValue pointers.
		 */
		inline std::vector<SUniformValue*>& getUniformParams()
		{
			return m_uniformParams;
		}

		/**
		 * @brief Get a reference to the uniform texture parameters vector.
		 * @return Reference to vector of SUniformTexture pointers.
		 */
		inline std::vector<SUniformTexture*>& getUniformTexture()
		{
			return m_uniformTextures;
		}

		/**
		 * @brief Get a reference to the extra parameters vector.
		 * @return Reference to vector of SExtraParams pointers.
		 */
		inline std::vector<SExtraParams*>& getExtraParams()
		{
			return m_extras;
		}

		/**
		 * @brief Enable or disable backface culling for the material.
		 * @param b True to enable, false to disable.
		 */
		inline void setBackfaceCulling(bool b)
		{
			m_backfaceCulling = b;
		}

		/**
		 * @brief Returns whether backface culling is enabled.
		 * @return True if enabled, false otherwise.
		 */
		inline bool isBackfaceCulling()
		{
			return m_backfaceCulling;
		}

		/**
		 * @brief Enable or disable frontface culling for the material.
		 * @param b True to enable, false to disable.
		 */
		inline void setFrontfaceCulling(bool b)
		{
			m_frontfaceCulling = b;
		}

		/**
		 * @brief Returns whether frontface culling is enabled.
		 * @return True if enabled, false otherwise.
		 */
		inline bool isFrontfaceCulling()
		{
			return m_frontfaceCulling;
		}

		/**
		 * @brief Enable or disable Z-buffer writing for the material.
		 * @param b True to enable, false to disable.
		 */
		inline void setZWrite(bool b)
		{
			m_zWriteEnable = b;
		}

		/**
		 * @brief Returns whether Z-buffer writing is enabled.
		 * @return True if enabled, false otherwise.
		 */
		inline bool isZWrite()
		{
			return m_zWriteEnable;
		}

		/**
		 * @brief Set the Z-buffer comparison function.
		 * @param f Comparison function value.
		 */
		inline void setZTest(video::E_COMPARISON_FUNC f)
		{
			m_zBuffer = f;
		}

		/**
		 * @brief Get the Z-buffer comparison function.
		 * @return Comparison function value.
		 */
		inline video::E_COMPARISON_FUNC getZTest()
		{
			return m_zBuffer;
		}

		/**
		 * @brief Get a uniform value by name (creates if not found).
		 * @param name Name of the uniform.
		 * @return Pointer to the SUniformValue instance.
		 */
		SUniformValue* getUniform(const char* name);

		/**
		 * @brief Get a uniform texture by name (creates if not found).
		 * @param name Name of the uniform texture.
		 * @return Pointer to SUniformTexture instance.
		 */
		SUniformTexture* getUniformTexture(const char* name);

		/**
		 * @brief Get an extra uniform value by name (creates if not found).
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform name.
		 * @return Pointer to SUniformValue instance.
		 */
		SUniformValue* getExtraUniform(SExtraParams* e, const char* name);

		/**
		 * @brief Get an extra uniform texture by name (creates if not found).
		 * @param e Pointer to extra parameter block.
		 * @param name Uniform texture name.
		 * @return Pointer to SUniformTexture instance.
		 */
		SUniformTexture* getExtraUniformTexture(SExtraParams* e, const char* name);

		/**
		 * @brief Get the shader parameter set for this material.
		 * @return Reference to CShaderParams instance.
		 */
		inline CShaderParams& getShaderParams()
		{
			return m_shaderParams;
		}

		/**
		 * @brief Returns true if the uniform with the given name exists.
		 * @param name Uniform name.
		 * @return True if exists, false otherwise.
		 */
		bool haveUniform(const char* name);

		/**
		 * @brief Initializes the material by loading the shader and setting up all parameters.
		 */
		void initMaterial();

		/**
		 * @brief Set textures for all slots using an array.
		 * @param textures Array of texture pointers.
		 * @param num Number of textures.
		 */
		void setTexture(ITexture** textures, int num);

		/**
		 * @brief Set the texture for a specific slot.
		 * @param slot Slot index.
		 * @param texture Pointer to texture.
		 */
		void setTexture(int slot, ITexture* texture);

		/**
		 * @brief Get the texture assigned to a slot.
		 * @param slot Slot index.
		 * @return Pointer to texture.
		 */
		ITexture* getTexture(int slot);

		/**
		 * @brief Set a material property by name.
		 * @param name Property name.
		 * @param value Property value.
		 */
		void setProperty(const std::string& name, const std::string& value);

		/**
		 * @brief Get a material property value by name.
		 * @param name Property name.
		 * @return Property value string.
		 */
		std::string getProperty(const std::string& name);

		/**
		 * @brief Load default textures specified by the shader.
		 */
		void loadDefaultTexture();

		/**
		 * @brief Load all uniform textures assigned to this material.
		 */
		void loadUniformTexture();

		/**
		 * @brief Unload all default textures.
		 */
		void unloadDefaultTexture();

		/**
		 * @brief Unload all uniform textures.
		 */
		void unloadUniformTexture();

		/**
		 * @brief Unload a specific uniform texture by name.
		 * @param name Uniform texture name.
		 */
		void unloadUniformTexture(const char* name);

		/**
		 * @brief Override a resource texture for this material.
		 * @param texture Pointer to texture.
		 * @param type Resource type (see CShader::EResourceType).
		 */
		void setOverrideResource(ITexture* texture, CShader::EResourceType type)
		{
			m_overrideTextures[type] = texture;
		}

	public:
		/**
		 * @brief Change the shader for this material.
		 * @param shader Pointer to new shader.
		 */
		void changeShader(CShader* shader);

		/**
		 * @brief Change the shader for this material using a path.
		 * @param path Path to new shader file.
		 */
		void changeShader(const char* path);

		/**
		 * @brief Automatically detect and load textures based on shader and uniform settings.
		 * @return True if all textures are found and loaded, false otherwise.
		 */
		bool autoDetectLoadTexture();

		/**
		 * @brief Apply material properties and textures internally (no arguments).
		 */
		void applyMaterial();

		/**
		 * @brief Update textures for the passed SMaterial.
		 * @param mat Reference to SMaterial.
		 */
		void applyMaterial(SMaterial& mat);

		/**
		 * @brief Replace a texture resource with a new texture in all relevant slots and uniforms.
		 * @param oldTexture Pointer to old texture.
		 * @param newTexture Pointer to new texture.
		 */
		void updateTexture(SMaterial& mat);

		/**
		 * @brief Replace a texture resource with a new texture in all relevant slots and uniforms.
		 * @param oldTexture Pointer to old texture.
		 * @param newTexture Pointer to new texture.
		 */
		void replaceTexture(ITexture* oldTexture, ITexture* newTexture);

		/**
		 * @brief Update internal shader parameter values based on uniforms.
		 */
		void updateShaderParams();

	protected:
		/**
		 * @brief Bind uniform parameter slots to shader uniforms.
		 */
		void bindUniformParam();

		/**
		 * @brief Update and assign texture slots for all uniforms.
		 */
		void updateSetTextureSlot();

		/**
		 * @brief Initialize default values for material uniforms based on shader defaults.
		 */
		void initDefaultValue();

		/**
		 * @brief Set default value for a uniform value using shader information.
		 * @param v Pointer to SUniformValue.
		 * @param u Pointer to SUniform.
		 */
		void setDefaultValue(SUniformValue* v, SUniform* u);

		/**
		 * @brief Create a new uniform value with given name and float size.
		 * @param name Name of the uniform.
		 * @param floatSize Number of floats.
		 * @return Pointer to new SUniformValue.
		 */
		SUniformValue* newUniform(const char* name, int floatSize);

		/**
		 * @brief Create a new uniform texture with given name.
		 * @param name Name of the uniform texture.
		 * @return Pointer to new SUniformTexture.
		 */
		SUniformTexture* newUniformTexture(const char* name);

		/**
		 * @brief Create a new extra uniform texture for a specific extra parameter block.
		 * @param e Pointer to extra parameter block.
		 * @param name Name of the uniform texture.
		 * @return Pointer to new SUniformTexture.
		 */
		SUniformTexture* newExtraUniformTexture(SExtraParams* e, const char* name);

		/**
		 * @brief Create a new extra uniform value for a specific extra parameter block.
		 * @param e Pointer to extra parameter block.
		 * @param name Name of the uniform.
		 * @param floatSize Number of floats.
		 * @return Pointer to new SUniformValue.
		 */
		SUniformValue* newExtraUniform(SExtraParams* e, const char* name, int floatSize);

		/**
		 * @brief Add a shader UI control (from shader XML) to the material's uniforms.
		 * @param ui Pointer to uniform UI definition.
		 */
		void addShaderUI(CShader::SUniformUI* ui);

		/**
		 * @brief Get extra parameter block by shader path.
		 * @param shaderPath Shader path string.
		 * @return Pointer to SExtraParams instance, or NULL if not found.
		 */
		SExtraParams* getExtraParams(const char* shaderPath);

		/**
		 * @brief Save extra parameters for the current shader path.
		 */
		void saveExtraParams();

		/**
		 * @brief Reload extra parameters for the given shader path.
		 * @param shaderPath Shader path string.
		 */
		void reloadExtraParams(const char* shaderPath);

		/**
		 * @brief Find an extra texture by name among all extra parameter blocks.
		 * @param name Uniform texture name.
		 * @return Pointer to SUniformTexture, or NULL if not found.
		 */
		SUniformTexture* findExtraTexture(const char* name);

		/**
		 * @brief Find an extra uniform value by name and float size among all extra parameter blocks.
		 * @param name Uniform name.
		 * @param floatSize Number of floats.
		 * @return Pointer to SUniformValue, or NULL if not found.
		 */
		SUniformValue* findExtraParam(const char* name, int floatSize);
	};

	typedef std::vector<CMaterial*> ArrayMaterial;
}