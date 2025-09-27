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

#include "pch.h"
#include "Utils/CSingleton.h"

#include <functional>

namespace Skylicht
{

	class CShader;
	class IShaderCallback;
	class IShaderInstancing;

	/// @brief Centralized manager for loading, caching, rebuilding, and controlling shader objects in Skylicht-Engine.
	/// @ingroup Materials
	/// 
	/// Handles initialization of built - in shaders for various rendering pipelines(basic, deferred, forward, mobile, PBR, etc.),
	/// manages shader instancing, uniform storage, and provides lookup functions for accessing shaders by name, path, or ID.
	/// 
	/// Allows for dynamic shader loading, rebuilding, and provides access to current rendering context (mesh / material).
	/// 
	/// Example usage :
	/// @code
	/// CShaderManager *shaderMgr = CShaderManager::getInstance();
	/// shaderMgr->loadShader("BuiltIn/Shader/Transparent/Transparent.xml");
	/// @endcode
	class SKYLICHT_API CShaderManager
	{
	public:
		DECLARE_SINGLETON(CShaderManager)

	protected:

		/// Pointer to mesh buffer currently being rendered
		IMeshBuffer* m_currentMeshBuffer;

		/// Pointer to material currently being rendered
		SMaterial* m_currentMatRendering;

		/// List of loaded shaders
		std::vector<CShader*> m_listShader;

		/// Map from shader name to internal material ID
		std::map<std::string, int> m_listShaderID;

	public:
		// Uniform storage for current draw command (used by shaders)

		/// Array for vec2 uniforms(max 10)
		/// @see EUniformType, SHADER_VEC2
		core::vector2df ShaderVec2[10];

		/// Array for vec3 uniforms(max 10)
		/// @see EUniformType, SHADER_VEC3
		core::vector3df ShaderVec3[10];

		/// Array for vec4 uniforms(max 10)
		/// @see EUniformType, SHADER_VEC4
		video::SVec4 ShaderVec4[10];

		/// Pointer to bone matrix array for skinning
		/// @see EUniformType, BONE_MATRIX
		f32* BoneMatrix;

		/// Number of bones for skinning
		/// @see EUniformType, BONE_COUNT
		u32 BoneCount;

		/// Current lightmap index
		/// @see EUniformType, LIGHTMAP_INDEX
		float LightmapIndex;

		/// Callback for custom instancing implementation
		/// See documentation page : @ref md__material_2_shader_2_r_e_a_d_m_e "Shader information structure"
		std::function<IShaderInstancing* (const char*)> OnCreateInstancingVertex;

	public:
		/**
		 * @brief Constructor.
		 */
		CShaderManager();

		/**
		 * @brief Destructor. Releases all loaded shaders.
		 */
		virtual ~CShaderManager();

		/**
		 * @brief Release all loaded shaders and associated resources.
		 */
		void releaseAll();

		/**
		 * @brief Set the mesh buffer to be used for current rendering.
		 * @param buffer Pointer to mesh buffer.
		 */
		inline void setCurrentMeshBuffer(IMeshBuffer* buffer)
		{
			m_currentMeshBuffer = buffer;
		}

		/**
		 * @brief Get the mesh buffer currently used for rendering.
		 * @return Pointer to current mesh buffer.
		 */
		inline IMeshBuffer* getCurrentMeshBuffer()
		{
			return m_currentMeshBuffer;
		}

		/**
		 * @brief Set the material to be used for current rendering.
		 * @param mat Reference to material.
		 */
		inline void setCurrentMaterial(SMaterial& mat)
		{
			m_currentMatRendering = &mat;
		}

		/**
		 * @brief Get the material currently used for rendering.
		 * @return Pointer to current material.
		 */
		inline SMaterial* getCurrentMaterial()
		{
			return m_currentMatRendering;
		}

		/**
		 * @brief Initialize built-in GUI shaders required for Skylicht's UI rendering.
		 */
		void initGUIShader();

		/**
		 * @brief Initialize basic built-in shaders for basic rendering features.
		 */
		void initBasicShader();

		/**
		 * @brief Initialize built-in shaders for Specular-Glossiness Deferred rendering.
		 */
		void initSGDeferredShader();

		/**
		 * @brief Initialize built-in shaders for Specular-Glossiness Forward rendering.
		 */
		void initSGForwarderShader();

		/**
		 * @brief Initialize built-in shaders for mobile platforms.
		 */
		void initMobileSGShader();

		/**
		 * @brief Initialize built-in shaders for Physically-Based Rendering (PBR) forward pipeline.
		 */
		void initPBRForwarderShader();

		/**
		 * @brief Initialize all core engine shaders (deferred, forward, etc.).
		 */
		void initSkylichtEngineShader();

		/**
		 * @brief Initialize all shader systems (calls basic and engine shader initializers).
		 */
		void initShader();

		/**
		 * @brief Load a shader from an XML configuration file.
		 *        If the shader is already loaded, returns the cached instance.
		 * @param shaderConfig Path to shader XML file.
		 * @return Pointer to loaded CShader instance, or NULL if not found.
		 */
		CShader* loadShader(const char* shaderConfig);

		/**
		 * @brief Rebuild a shader by reloading its configuration from source file.
		 *        Useful for hot-reloading, debugging, or updating shaders at runtime.
		 * @param shader Pointer to shader to rebuild.
		 * @return True if successful, false otherwise.
		 */
		bool rebuildShader(CShader* shader);

		/**
		 * @brief Get the material ID of a shader by its name.
		 * @param name Name of the shader.
		 * @return Internal material ID, or 0 if not found.
		 */
		int getShaderIDByName(const char* name);

		/**
		 * @brief Get a shader instance by its name.
		 * @param name Name of the shader.
		 * @return Pointer to the CShader instance, or NULL if not found.
		 */
		CShader* getShaderByName(const char* name);

		/**
		 * @brief Get a shader instance by its source path.
		 * @param path Path to shader source/config file.
		 * @return Pointer to the CShader instance, or NULL if not found.
		 */
		CShader* getShaderByPath(const char* path);

		/**
		 * @brief Get a shader instance by its internal material ID.
		 * @param id Internal material ID.
		 * @return Pointer to the CShader instance, or NULL if not found.
		 */
		CShader* getShaderByID(int id);

		/**
		 * @brief Get the shader file name with the appropriate extension for the current graphics API.
		 *        Appends .hlsl for Direct3D, .glsl for OpenGL.
		 * @param fileName Base file name.
		 * @return Shader file name with extension.
		 */
		std::string getShaderFileName(const char* fileName);

		/**
		 * @brief Get the number of loaded shader materials.
		 * @return Number of loaded shaders.
		 */
		inline int getNumShader()
		{
			return (int)m_listShader.size();
		}

		/**
		 * @brief Get a loaded shader material by index.
		 * @param i Index of the shader.
		 * @return Pointer to the CShader instance.
		 */
		CShader* getShader(int i)
		{
			return m_listShader[i];
		}

	protected:

		/**
		 * @brief Internal function to build a shader from XML, source, and folder.
		 *        Handles dependency loading, instancing setup, and error logging.
		 * @param shader Pointer to shader to build.
		 * @param xmlReader XML reader for shader config.
		 * @param source Source path.
		 * @param shaderFolder Shader folder path.
		 * @param rebuild True if rebuilding, false if initial load.
		 * @return True if successful, false otherwise.
		 */
		bool buildShader(CShader* shader, io::IXMLReader* xmlReader, const char* source, const char* shaderFolder, bool rebuild);
	};

}