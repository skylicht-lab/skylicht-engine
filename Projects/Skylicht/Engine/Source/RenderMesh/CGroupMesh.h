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
#pragma once

#include "Entity/CEntityGroup.h"

namespace Skylicht
{
	class CGroupMesh : public CEntityGroup
	{
	protected:
		CFastArray<CEntity*> m_staticMesh;
		CFastArray<CEntity*> m_skinnedMesh;
		CFastArray<CEntity*> m_blendShapeMesh;
		CFastArray<CEntity*> m_softwareSkinnedMesh;
		CFastArray<CEntity*> m_hardwareSkinnedMesh;
		CFastArray<CEntity*> m_instancingMesh;

	public:
		CGroupMesh(CEntityGroup* parent);

		virtual ~CGroupMesh();

		inline int getNumStaticMesh()
		{
			return m_staticMesh.count();
		}

		inline int getNumSkinnedMesh()
		{
			return m_skinnedMesh.count();
		}

		inline int getNumBlendShape()
		{
			return m_blendShapeMesh.count();
		}

		inline int getNumSoftwareSkinnedMesh()
		{
			return m_softwareSkinnedMesh.count();
		}

		inline int getNumHardwareSkinnedMesh()
		{
			return m_hardwareSkinnedMesh.count();
		}

		inline int getNumInstancingMesh()
		{
			return m_instancingMesh.count();
		}

		inline CEntity** getStaticMeshes()
		{
			return m_staticMesh.pointer();
		}

		inline CEntity** getSkinnedMeshes()
		{
			return m_skinnedMesh.pointer();
		}

		inline CEntity** getBlendShapeMeshes()
		{
			return m_blendShapeMesh.pointer();
		}

		inline CEntity** getSoftwareSkinnedMeshes()
		{
			return m_softwareSkinnedMesh.pointer();
		}

		inline CEntity** getHardwareSkinnedMeshes()
		{
			return m_hardwareSkinnedMesh.pointer();
		}

		inline CEntity** getInstancingMeshes()
		{
			return m_instancingMesh.pointer();
		}

		virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity);
	};
}