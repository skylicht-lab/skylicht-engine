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

#include "pch.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"
#include "Lighting/CSpotLight.h"

#include "RenderMesh/CRenderMesh.h"
#include "RenderMesh/CRenderMeshInstancing.h"
#include "OcclusionQuery/COcclusionQuery.h"
#include "IndirectLighting/CIndirectLighting.h"
#include "LightProbes/CLightProbes.h"

#include "SkyDome/CSkyDome.h"
#include "SkyBox/CSkyBox.h"

#include "Primitive/CCube.h"
#include "Primitive/CSphere.h"
#include "Primitive/CPlane.h"

#ifdef BUILD_SKYLICHT_PHYSIC
#include "Collider/CBoxCollider.h"
#include "Collider/CStaticPlaneCollider.h"
#include "Collider/CBvhMeshCollider.h"
#include "Collider/CConvexMeshCollider.h"
#include "Collider/CMeshCollider.h"
#include "Collider/CCylinderCollider.h"
#include "Collider/CCapsuleCollider.h"
#include "Collider/CSphereCollider.h"
#include "RigidBody/CRigidbody.h"
using namespace Physics;
#endif

#define USE_COMPONENT(component) CComponentSystem *component##_used = addComponent(new component())

namespace Skylicht
{
	namespace Editor
	{
		std::vector<CComponentSystem*> g_component_used;

		CComponentSystem* addComponent(CComponentSystem* add)
		{
			g_component_used.push_back(add);
			return add;
		}

		int cleanUp()
		{
			for (CComponentSystem* comp : g_component_used)
				delete comp;

			g_component_used.clear();
			return 0;
		}

		// BEGIN DECLARE COMPONENT THAT WILL COMPILE

		USE_COMPONENT(CDirectionalLight);
		USE_COMPONENT(CPointLight);
		USE_COMPONENT(CSpotLight);

		USE_COMPONENT(CRenderMesh);
		USE_COMPONENT(CRenderMeshInstancing);
		USE_COMPONENT(COcclusionQuery);

		USE_COMPONENT(CSkyDome);
		USE_COMPONENT(CSkyBox);

		USE_COMPONENT(CIndirectLighting);
		USE_COMPONENT(CLightProbes);

		USE_COMPONENT(CCube);
		USE_COMPONENT(CSphere);
		USE_COMPONENT(CPlane);

#ifdef BUILD_SKYLICHT_PHYSIC
		USE_COMPONENT(CRigidbody);
		USE_COMPONENT(CBoxCollider);
		USE_COMPONENT(CStaticPlaneCollider);
		USE_COMPONENT(CMeshCollider);
		USE_COMPONENT(CBvhMeshCollider);
		USE_COMPONENT(CConvexMeshCollider);
		USE_COMPONENT(CCylinderCollider);
		USE_COMPONENT(CCapsuleCollider);
		USE_COMPONENT(CSphereCollider);
#endif

		// END DECLARE COMPONENT

		int clean = cleanUp();
	}
}