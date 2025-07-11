/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "Utils/CSingleton.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	namespace Editor
	{
		class CHandlesRenderer;
		class CGizmosRenderer;

		class CHandles : public IEventReceiver
		{
		public:
			DECLARE_SINGLETON(CHandles)

		protected:
			core::matrix4 m_world;
			core::matrix4 m_worldInv;

			core::vector3df m_position;
			core::quaternion m_rotation;
			core::vector3df m_scale;

			bool m_handleListPosition;
			bool m_handlePosition;
			bool m_handleRotation;
			bool m_handleScale;

			core::vector3df m_targetPosition;
			core::quaternion m_targetRotation;
			core::vector3df m_targetScale;

			core::quaternion m_rotationOrigin;

			std::vector<core::vector3df> m_listPosition;
			int m_selectId;

			int m_mouseState;

			bool m_endCheck;
			bool m_useLocalSpace;

			bool m_snapXZ;
			bool m_snapY;
			bool m_snapRotate;
			float m_snapDistanceXZ;
			float m_snapDistanceY;
			float m_snapRotateDeg;

			bool m_isAltPressed;

			CHandlesRenderer* m_handlesRenderer;
			CGizmosRenderer* m_gizmosRenderer;

		public:
			CHandles();

			virtual ~CHandles();

			bool isEnable();

			bool endCheck();

			void end();

			void reset();

			void refresh();

			void setNullRenderer();

			bool listPositionHandle(const std::vector<core::vector3df>& pos, int selectId, const core::quaternion& localRotation);

			core::vector3df positionHandle(const core::vector3df& position, const core::quaternion& localRotation);

			core::vector3df scaleHandle(const core::vector3df& scale, const core::vector3df& origin, const core::quaternion& localRotation);

			core::quaternion rotateHandle(const core::quaternion& rotate, const core::vector3df& origin);

			inline std::vector<core::vector3df>& getListPosition()
			{
				return m_listPosition;
			}

			inline int getSelectId()
			{
				return m_selectId;
			}

			inline void setSelectId(int id)
			{
				m_selectId = id;
			}

			inline void setUseLocalSpace(bool b)
			{
				m_useLocalSpace = b;
			}

			inline bool useLocalSpace()
			{
				return m_useLocalSpace;
			}

			inline void snapXZ(bool b)
			{
				m_snapXZ = b;
			}

			inline bool isSnapXZ()
			{
				return m_snapXZ;
			}

			inline void snapY(bool b)
			{
				m_snapY = b;
			}

			inline bool isSnapY()
			{
				return m_snapY;
			}

			inline void snapRotate(bool b)
			{
				m_snapRotate = b;
			}

			inline bool isSnapRotate()
			{
				return m_snapRotate;
			}

			inline void setSnapDistanceXZ(float d)
			{
				m_snapDistanceXZ = d;
			}

			inline float getSnapDistanceXZ()
			{
				return m_snapDistanceXZ;
			}

			inline void setSnapDistanceY(float d)
			{
				m_snapDistanceY = d;
			}

			inline float getSnapDistanceY()
			{
				return m_snapDistanceY;
			}

			inline void setSnapRotateDeg(float d)
			{
				m_snapRotateDeg = d;
			}

			inline float getSnapRotateDeg()
			{
				return m_snapRotateDeg;
			}

			virtual bool OnEvent(const SEvent& event);

		public:

			inline bool isHandlePosition()
			{
				return m_handlePosition;
			}

			inline bool isHandleRotation()
			{
				return m_handleRotation;
			}

			inline bool isHandleScale()
			{
				return m_handleScale;
			}

			inline bool isHandleListPosition()
			{
				return m_handleListPosition;
			}

			const core::vector3df& getHandlePosition();

			void setTargetPosition(const core::vector3df& target);

			inline const core::quaternion& getHandleRotation()
			{
				return m_rotation;
			}

			inline const core::quaternion& getRotationOrigin()
			{
				return m_rotationOrigin;
			}

			inline void setTargetRotation(const core::quaternion& target)
			{
				m_targetRotation = target;
			}

			inline const core::vector3df& getHandleScale()
			{
				return m_scale;
			}

			inline void setTargetScale(const core::vector3df& scale)
			{
				m_targetScale = scale;
			}

			inline void setEndCheck(bool b)
			{
				m_endCheck = b;
			}

			inline void setWorld(const core::matrix4& m)
			{
				m_world = m;
				m.getInverse(m_worldInv);
			}

			inline core::matrix4& getWorld()
			{
				return m_world;
			}

			inline core::matrix4& getWorldInv()
			{
				return m_worldInv;
			}

			bool isHoverOnAxisOrPlane();

			bool isUsing();

		public:

			void draw3DBox(const core::aabbox3d<f32>& box, const SColor& color);

			void drawCylinder(const core::vector3df& pos, const core::vector3df& direction, float length, float radius, const SColor& color);

			void drawSphere(const core::vector3df& pos, float radius, const SColor& color);

			void drawLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color);

			void drawPolyline(const core::vector3df* points, u32 count, bool close, const SColor& color);

			void drawCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color);

			void drawRectBillboard(const core::vector3df& pos, float w, float h, const SColor& color);

			void drawArrowInViewSpace(const core::vector3df& pos, const core::vector3df& v, float length, float arrowSize, const SColor& color);
		};
	}
}