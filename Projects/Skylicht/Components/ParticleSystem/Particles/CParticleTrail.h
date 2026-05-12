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
			/** @brief The group whose particles generate these trails. */
			CGroup* m_group;

			/** @brief Friendly name. */
			std::wstring m_name;

			/** @brief Active trails for live particles. */
			core::array<STrailInfo> m_trails;

			/** @brief Fading trails for dead particles. */
			core::array<STrailInfo> m_deadTrails;

			/** @brief Internal mesh buffer for the trail ribbon. */
			IMeshBuffer* m_meshBuffer;

			/** @brief Distance between ribbon segments. */
			float m_segmentLength;

			/** @brief Max number of segments per trail. */
			u32 m_maxSegmentCount;

			/** @brief Ribbon width. */
			float m_width;

			/** @brief Total trail length in units. */
			float m_length;

			/** @brief Current count of active trails. */
			u32 m_trailCount;

			/** @brief Whether to kill the trail immediately when its particle dies. */
			bool m_destroyWhenParticleDead;

			/** @brief Fading speed for dead trails. */
			float m_deadAlphaReduction;

			/** @brief Path to the ribbon texture. */
			std::string m_texturePath;

			/** @brief Default material for the ribbon. */
			CMaterial* m_material;

			/** @brief Optional custom material. */
			CMaterial* m_customMaterial;

			/** @brief Whether to use custom material. */
			bool m_useCustomMaterial;

			/** @brief Local-to-world transform. */
			core::matrix4 m_world;

			/** @brief Whether the trail has emission. */
			bool m_emission;

			/** @brief Emission intensity multiplier. */
			float m_emissionIntensity;

			/** @brief Whether the ribbon always faces the camera. */
			bool m_billboard;

			/** @brief Custom up vector if billboarding is disabled. */
			core::vector3df m_upVector;

		public:
			CParticleTrail(CGroup* group);

			virtual ~CParticleTrail();

			/** @brief Updates ribbon geometry based on particle history and camera position. */
			virtual void update(CCamera* camera);

			/** @brief Callback: records new particle positions into trails. */
			virtual void OnParticleUpdate(CParticle* particles, int num, CGroup* group, float dt);

			/** @brief Callback: initializes trail for a new particle. */
			virtual void OnParticleBorn(CParticle& p);

			/** @brief Callback: moves trail to dead list for fading or removes it. */
			virtual void OnParticleDead(CParticle& p);

			/** @brief Callback: swaps trail indices when particles are reordered. */
			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2);

			/** @brief Callback: handles parent group destruction. */
			virtual void OnGroupDestroy();

			/** @brief Gets raw access to trail mesh. */
			inline IMeshBuffer* getMeshBuffer()
			{
				return m_meshBuffer;
			}

			/** @brief Gets active material. */
			inline CMaterial* getMaterial()
			{
				return m_material;
			}

			/** @brief Sets ribbon width. */
			inline void setWidth(float f)
			{
				m_width = f;
			}

			/** @brief Gets ribbon width. */
			inline float getWidth()
			{
				return m_width;
			}

			/** @brief Sets segment separation distance. */
			inline void setSegmentLength(float f)
			{
				m_segmentLength = f;
			}

			/** @brief Gets segment separation distance. */
			inline float getSegmentLength()
			{
				return m_segmentLength;
			}

			/** @brief Checks death destruction policy. */
			inline bool isDestroyedWhenParticleDead()
			{
				return m_destroyWhenParticleDead;
			}

			/** @brief Configures death destruction policy. */
			inline void enableDestroyWhenParticleDead(bool b)
			{
				m_destroyWhenParticleDead = b;
			}

			/** @brief Gets fading speed for dead trails. */
			inline float getDeadAlphaReduction()
			{
				return m_deadAlphaReduction;
			}

			/** @brief Sets fading speed for dead trails. */
			inline void setDeadAlphaReduction(float a)
			{
				m_deadAlphaReduction = a;
			}

			/** @brief Enables/disables custom material. */
			inline void enableCustomMaterial(bool b)
			{
				m_useCustomMaterial = b;
			}

			/** @brief Sets custom material. */
			inline void setCustomMaterial(CMaterial* material)
			{
				m_customMaterial = material;
			}

			/** @brief Checks if custom material is active. */
			inline bool useCustomMaterial()
			{
				return m_useCustomMaterial;
			}

			/** @brief Gets path to custom material. */
			inline const char* getCustomMaterial()
			{
				if (m_customMaterial)
					return m_customMaterial->getMaterialPath();
				return "";
			}

			/** @brief Gets name of linked group. */
			inline const wchar_t* getGroupName()
			{
				if (m_group)
					return m_group->Name.c_str();
				return m_name.c_str();
			}

			/** @brief Sets max trail length. */
			void setLength(float l);

			/** @brief Gets max trail length. */
			inline float getLength()
			{
				return m_length;
			}

			/** @brief Directly sets ribbon texture. */
			void setTexture(ITexture* texture);

			/** @brief Updates material parameters in the mesh buffer. */
			void applyMaterial();

			/** @brief Loads ribbon texture by path. */
			void setTexturePath(const char* path);

			/** @brief Gets ribbon texture path. */
			inline const char* getTexturePath()
			{
				return m_texturePath.c_str();
			}

			/** @brief Enables/disables emission. */
			inline void setEmission(bool b)
			{
				m_emission = b;
			}

			/** @brief Checks if emission is enabled. */
			inline bool isEmission()
			{
				return m_emission;
			}

			/** @brief Sets emission intensity. */
			inline void setEmissionIntensity(float f)
			{
				m_emissionIntensity = f;
			}

			/** @brief Gets emission intensity. */
			inline float getEmissionIntensity()
			{
				return m_emissionIntensity;
			}

			/** @brief Enables/disables camera billboarding. */
			inline void setBillboard(bool b)
			{
				m_billboard = b;
			}

			/** @brief Checks if billboarding is enabled. */
			inline bool isBillboard()
			{
				return m_billboard;
			}

			/** @brief Sets custom up vector for orientation. */
			inline void setUpVector(const core::vector3df& up)
			{
				m_upVector = up;
			}

			/** @brief Gets custom up vector. */
			const core::vector3df& getUpVector()
			{
				return m_upVector;
			}

		protected:

			/** @brief Internal: manages fading of dead trails. */
			void updateDeadTrail();

			/** @brief Internal: sets world matrix. */
			void setWorld(const core::matrix4& world)
			{
				m_world = world;
			}

		};
	}
}