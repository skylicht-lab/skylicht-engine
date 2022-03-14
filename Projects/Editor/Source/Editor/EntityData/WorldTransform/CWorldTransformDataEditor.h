/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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

#include "Editor/EntityData/CEntityDataEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Gizmos/Transform/CWorldTransformDataGizmos.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	namespace Editor
	{
		class CWorldTransformDataEditor : public CEntityDataEditor
		{
		protected:
			CWorldTransformDataGizmos* m_gizmos;

			CWorldTransformData* m_worldTransform;

			CSubject<float> X;
			CSubject<float> Y;
			CSubject<float> Z;

			CSubject<float> ScaleX;
			CSubject<float> ScaleY;
			CSubject<float> ScaleZ;

			CSubject<float> RotateX;
			CSubject<float> RotateY;
			CSubject<float> RotateZ;

			core::vector3df m_position;
			core::vector3df m_rotate;
			core::vector3df m_scale;

		public:
			CWorldTransformDataEditor();

			virtual ~CWorldTransformDataEditor();

			virtual void initGUI(IEntityData* entityData, CSpaceProperty* ui);

			virtual void closeGUI();

			virtual void update();

			void updateMatrix();
		};
	}
}