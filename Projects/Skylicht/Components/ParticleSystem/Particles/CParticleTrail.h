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

		public:
			CParticleTrail(CGroup* group);

			virtual ~CParticleTrail();

			virtual void update(CCamera* camera);

			virtual void OnParticleUpdate(CParticle* particles, int num, CGroup* group, float dt);

			virtual void OnParticleBorn(CParticle& p);

			virtual void OnParticleDead(CParticle& p);

			virtual void OnSwapParticleData(CParticle& p1, CParticle& p2);

			virtual void OnGroupDestroy();

			inline IMeshBuffer* getMeshBuffer()
			{
				return m_meshBuffer;
			}

			inline CMaterial* getMaterial()
			{
				return m_material;
			}

			inline void setWidth(float f)
			{
				m_width = f;
			}

			inline float getWidth()
			{
				return m_width;
			}

			inline void setSegmentLength(float f)
			{
				m_segmentLength = f;
			}

			inline float getSegmentLength()
			{
				return m_segmentLength;
			}

			inline bool isDestroyedWhenParticleDead()
			{
				return m_destroyWhenParticleDead;
			}

			inline void enableDestroyWhenParticleDead(bool b)
			{
				m_destroyWhenParticleDead = b;
			}

			inline float getDeadAlphaReduction()
			{
				return m_deadAlphaReduction;
			}

			inline void setDeadAlphaReduction(float a)
			{
				m_deadAlphaReduction = a;
			}

			inline void enableCustomMaterial(bool b)
			{
				m_useCustomMaterial = b;
			}

			inline void setCustomMaterial(CMaterial* material)
			{
				m_customMaterial = material;
			}

			inline bool useCustomMaterial()
			{
				return m_useCustomMaterial;
			}

			inline const char* getCustomMaterial()
			{
				if (m_customMaterial)
					return m_customMaterial->getMaterialPath();
				return "";
			}

			inline const wchar_t* getGroupName()
			{
				if (m_group)
					return m_group->Name.c_str();
				return m_name.c_str();
			}

			void setLength(float l);

			inline float getLength()
			{
				return m_length;
			}

			void setTexture(ITexture* texture);

			void applyMaterial();

			void setTexturePath(const char* path);

			inline const char* getTexturePath()
			{
				return m_texturePath.c_str();
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