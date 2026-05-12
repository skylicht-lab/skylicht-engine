/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CGroup.h"

namespace Skylicht
{
	class CCamera;

	namespace Particle
	{
		class CParticleTrailComponent;

		struct SParticlePosition
		{
			core::vector3df Position;
			float Width;
			float Alpha;
		};

		struct STrailInfo
		{
			core::array<SParticlePosition>* Position;
			core::vector3df CurrentPosition;
			core::vector3df LastPosition;
			video::SColor CurrentColor;
			int Flag;

			STrailInfo()
			{
				Flag = 0;
				Position = NULL;
			}

			void InitData()
			{
				Position = new core::array<SParticlePosition>();
			}

			void DeleteData()
			{
				if (Position)
				{
					delete Position;
					Position = NULL;
				}
			}

			void Copy(const STrailInfo& t)
			{
				CurrentPosition = t.CurrentPosition;
				LastPosition = t.LastPosition;
				CurrentColor = t.CurrentColor;
				Flag = t.Flag;
				*Position = *t.Position;
			}
		};

		class COMPONENT_API CParticleTrail : public IParticleCallback
		{
			friend class CParticleTrailComponent;

		protected:
			CGroup* m_group;

			std::wstring m_name;

			core::array<STrailInfo> m_trails;

			core::array<STrailInfo> m_deadTrails;

			IMeshBuffer* m_meshBuffer;

			float m_segmentLength;

			u32 m_maxSegmentCount;

			float m_width;

			float m_length;

			u32 m_trailCount;

			bool m_destroyWhenParticleDead;

			float m_deadAlphaReduction;

			std::string m_texturePath;

			CMaterial* m_material;

			CMaterial* m_customMaterial;

			bool m_useCustomMaterial;

			core::matrix4 m_world;

			bool m_emission;

			float m_emissionIntensity;

			bool m_billboard;

			core::vector3df m_upVector;

		public:
			CParticleTrail(CGroup* group);

			virtual ~CParticleTrail();

			/**
			 * @brief Update ribbon mesh buffers. Called by CParticleTrailRenderer.
			 * @param camera Active camera for billboarding.
			 */
			virtual void update(CCamera* camera);

			virtual void OnParticleUpdate(CParticle* particles, int num, CGroup* group, float dt);

			virtual void OnParticleBorn(CParticle& p);

			virtual void OnParticleDead(CParticle& p);

			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2);

			virtual void OnGroupDestroy();

			/**
			 * @brief Get the generated ribbon mesh buffer.
			 * @return Pointer to IMeshBuffer.
			 */
			inline IMeshBuffer* getMeshBuffer()
			{
				return m_meshBuffer;
			}

			/**
			 * @brief Get the trail material.
			 * @return Pointer to CMaterial.
			 */
			inline CMaterial* getMaterial()
			{
				return m_material;
			}

			/**
			 * @brief Set the width of the ribbon.
			 * @param f Width value.
			 */
			inline void setWidth(float f)
			{
				m_width = f;
			}

			/**
			 * @brief Get ribbon width.
			 * @return Width value.
			 */
			inline float getWidth()
			{
				return m_width;
			}

			/**
			 * @brief Set distance between trail segments.
			 * @param f Distance.
			 */
			inline void setSegmentLength(float f)
			{
				m_segmentLength = f;
			}

			/**
			 * @brief Get segment length.
			 * @return Distance value.
			 */
			inline float getSegmentLength()
			{
				return m_segmentLength;
			}

			/**
			 * @brief Check if the trail is destroyed immediately when the particle dies.
			 * @return True if destroyed.
			 */
			inline bool isDestroyedWhenParticleDead()
			{
				return m_destroyWhenParticleDead;
			}

			/**
			 * @brief Enable or disable immediate trail destruction upon particle death.
			 * @param b True to destroy immediately; false to allow the trail to fade out.
			 */
			inline void enableDestroyWhenParticleDead(bool b)
			{
				m_destroyWhenParticleDead = b;
			}

			/**
			 * @brief Get the alpha fading speed for dead trails.
			 * @return Fading speed.
			 */
			inline float getDeadAlphaReduction()
			{
				return m_deadAlphaReduction;
			}

			/**
			 * @brief Set the alpha fading speed for dead trails.
			 * @param a Fading speed.
			 */
			inline void setDeadAlphaReduction(float a)
			{
				m_deadAlphaReduction = a;
			}

			/**
			 * @brief Enable or disable custom material for the trail.
			 * @param b True to use custom material.
			 */
			inline void enableCustomMaterial(bool b)
			{
				m_useCustomMaterial = b;
			}

			/**
			 * @brief Set a custom material for the trail.
			 * @param material Pointer to CMaterial.
			 */
			inline void setCustomMaterial(CMaterial* material)
			{
				m_customMaterial = material;
			}

			/**
			 * @brief Check if custom material is enabled.
			 * @return True if enabled.
			 */
			inline bool useCustomMaterial()
			{
				return m_useCustomMaterial;
			}

			/**
			 * @brief Get the path to the custom material.
			 * @return Path string.
			 */
			inline const char* getCustomMaterial()
			{
				if (m_customMaterial)
					return m_customMaterial->getMaterialPath();
				return "";
			}

			/**
			 * @brief Get the name of the associated particle group.
			 * @return Wstring name.
			 */
			inline const wchar_t* getGroupName()
			{
				if (m_group)
					return m_group->Name.c_str();
				return m_name.c_str();
			}

			/**
			 * @brief Set the total visual length of the trail.
			 * @param l Length value.
			 */
			void setLength(float l);

			/**
			 * @brief Get total trail length.
			 * @return Length value.
			 */
			inline float getLength()
			{
				return m_length;
			}

			/**
			 * @brief Set texture for the ribbon.
			 * @param texture Pointer to ITexture.
			 */
			void setTexture(ITexture* texture);

			/**
			 * @brief Apply current settings to the mesh buffer material.
			 */
			void applyMaterial();

			/**
			 * @brief Load a texture from path.
			 * @param path Texture file path.
			 */
			void setTexturePath(const char* path);

			/**
			 * @brief Get active texture path.
			 * @return Path string.
			 */
			inline const char* getTexturePath()
			{
				return m_texturePath.c_str();
			}

			/**
			 * @brief Enable or disable emission pass.
			 * @param b True to enable.
			 */
			inline void setEmission(bool b)
			{
				m_emission = b;
			}

			/**
			 * @brief Check if emission is enabled.
			 * @return True if enabled.
			 */
			inline bool isEmission()
			{
				return m_emission;
			}

			/**
			 * @brief Set emission intensity for glowing effects.
			 * @param f Intensity value.
			 */
			inline void setEmissionIntensity(float f)
			{
				m_emissionIntensity = f;
			}

			/**
			 * @brief Get emission intensity.
			 * @return Intensity value.
			 */
			inline float getEmissionIntensity()
			{
				return m_emissionIntensity;
			}

			/**
			 * @brief Enable or disable billboarding (facing camera).
			 * @param b True to enable.
			 */
			inline void setBillboard(bool b)
			{
				m_billboard = b;
			}

			/**
			 * @brief Check if billboarding is enabled.
			 * @return True if enabled.
			 */
			inline bool isBillboard()
			{
				return m_billboard;
			}

			/**
			 * @brief Set the up-vector for non-billboarded ribbons.
			 * @param up Direction vector.
			 */
			inline void setUpVector(const core::vector3df& up)
			{
				m_upVector = up;
			}

			/**
			 * @brief Get current up-vector.
			 * @return Direction vector reference.
			 */
			const core::vector3df& getUpVector()
			{
				return m_upVector;
			}

		protected:

			void updateDeadTrail();

			void setWorld(const core::matrix4& world)
			{
				m_world = world;
			}

		};
	}
}