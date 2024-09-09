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

// Config
#include "SkylichtConfig.h"
#include "SkylichtSystemAPI.h"

#ifdef BUILD_SKYLICHT_AUDIO
#include "SkylichtAudioAPI.h"
#endif

#ifdef BUILD_SKYLICHT_COLLISION
#include "CollisionConfig.h"
#endif

#ifdef BUILD_SKYLICHT_COMPONENTS
#include "ComponentsConfig.h"
#endif

#include "CApplication.h"

// Build Info
#include "BuildConfig/CBuildConfig.h"

#include "Utils/CSingleton.h"

// Control
#include "Control/CTouchManager.h"
#include "Control/CAccelerometer.h"
#include "Control/CJoystick.h"

// Material
#include "Material/CMaterialManager.h"
#include "Material/Shader/CShaderManager.h"

// Event
#include "EventManager/CEventManager.h"

// Texture Manager
#include "TextureManager/CTextureManager.h"

// Render pipeline
#include "RenderPipeline/CForwardRP.h"
#include "RenderPipeline/CDeferredRP.h"
#include "RenderPipeline/CShadowMapRP.h"
#include "RenderPipeline/CPostProcessorRP.h"

// Scene, GameObject
#include "Scene/CScene.h"
#include "Camera/CCamera.h"
#include "Camera/CEditorCamera.h"
#include "Camera/CFpsMoveCamera.h"
#include "Camera/C3rdCamera.h"
#include "Camera/CCameraBrain.h"

#include "GameObject/CGameObject.h"
#include "GameObject/CZone.h"

#include "Lighting/CDirectionalLight.h"
#include "Lighting/CPointLight.h"
#include "IndirectLighting/CIndirectLighting.h"

#include "ReflectionProbe/CReflectionProbe.h"
#include "LightProbes/CLightProbe.h"

// Graphics2D, Canvas 2D
#include "Projective/CProjective.h"

#include "Graphics2D/CCanvas.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"
#include "Graphics2D/SpriteFrame/CSpriteFont.h"
#include "Graphics2D/SpriteFrame/CSpriteManager.h"
#include "Graphics2D/SpriteFrame/CFontManager.h"

// Font - Freetype
#include "Graphics2D/Glyph/CGlyphFreetype.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"

// ECS system
#include "Entity/CEntityManager.h"
#include "Entity/CEntityPrefab.h"

// Culling
#include "Culling/CCullingData.h"

// 3D Model
#include "MeshManager/CMeshManager.h"
#include "RenderMesh/CMesh.h"
#include "RenderMesh/CSkinnedMesh.h"
#include "RenderMesh/CRenderMesh.h"
#include "RenderMesh/CRenderMeshInstancing.h"

// OcclusionQuery
#include "OcclusionQuery/COcclusionQuery.h"

// 3D Animation
#include "Animation/CAnimationManager.h"
#include "Animation/CAnimationController.h"
#include "Animation/CAnimation.h"

// Transform
#include "Transform/CTransform.h"
#include "Transform/CTransformEuler.h"
#include "Transform/CTransformMatrix.h"

// Importer
#include "Importer/Utils/CMeshUtils.h"

// Serializable
#include "Serializable/CValueProperty.h"
#include "Serializable/CObjectSerializable.h"
#include "Serializable/CSerializableLoader.h"

// Utils
#include "Utils/CColor.h"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"
#include "Utils/CMemoryStream.h"
#include "Utils/XMLSpreadsheet/CXMLSpreadsheet.h"
#include "Utils/XMLSpreadsheet/CXMLTableData.h"

// Tween
#include "Tween/CTweenManager.h"
#include "Tween/CTweenColor.h"
#include "Tween/CTweenFloat.h"
#include "Tween/CTweenMatrix4.h"
#include "Tween/CTweenQuaternion.h"
#include "Tween/CTweenVector3df.h"
#include "Tween/CTweenVector2df.h"