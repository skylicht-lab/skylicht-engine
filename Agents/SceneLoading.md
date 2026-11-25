# Loading a .scene and preparing runtime objects

This guide follows [FirstInit.md](FirstInit.md) and [Architecture.md](Architecture.md). It describes loading an editor-authored scene into an independent Skylicht application, finding objects by layer, applying runtime materials and components, and preloading particle effects. All asset paths below are examples supplied by your application, except paths under `BuiltIn`.

## 1. Prepare the runtime

Initialize the engine, component libraries, shaders, and resource archives before importing. The `.scene` stores serialized objects and component properties; referenced models, textures, materials, lightmaps, and particle files must also be accessible through the Irrlicht filesystem. Mounting an archive makes its files available; it does not preload all of them.

Custom component types serialized by the editor must be registered and linked into the runtime before import. An unsupported component is logged and represented by `CNullComponent`; successful import does not prove every authored component is supported. Initialize optional subsystems, such as physics, before importing components that use them.

Relevant headers include:

```cpp
#include "Scene/CScene.h"
#include "Scene/CSceneImporter.h"
#include "GameObject/CZone.h"
#include "RenderMesh/CRenderMesh.h"
#include "Material/CMaterialManager.h"
#include "Lightmap/CLightmap.h"
#include "OcclusionQuery/COcclusionQuery.h"
#include "ParticleSystem/CParticleComponent.h"
```

Fragments use `Skylicht` names and the application's existing Irrlicht types. The particle component belongs to the optional Components library.

## 2. Import across loading frames

Use `CSceneImporter::beginImportScene(scene, path)` once, then `updateLoadScene()` once per loading update until it returns `true`. Keep rendering a separate loading scene while preparing the destination scene.

The importer uses process-global state, shared with template operations. Complete one import before beginning another scene/template operation. Keep the destination scene alive until completion. The public importer has no general cancellation API; do not delete a partially imported scene and continue calling the importer.

Import is incremental, not a background task. `beginImportScene()` scans the XML and creates the object/component hierarchy synchronously, then reopens the file. Each `updateLoadScene()` deserializes a batch (currently up to ten objects), loads component resources, and calls each object's `startComponent()`. An expensive object can still take a long frame. Do not move this work to `std::async`: resource creation and engine state require the normal engine thread.

The following are application loading-controller members and method bodies, not additional engine APIs:

```cpp
// Members: initialize before the first call.
CScene* m_pendingScene = nullptr;
bool m_importing = false;
bool m_importComplete = false;
float m_importProgress = 0.0f;

bool beginSceneLoad(const char* path)
{
	// Also serialize imports across all other loading controllers.
	if (m_pendingScene != nullptr)
		return false;

	m_pendingScene = new CScene();
	if (!CSceneImporter::beginImportScene(m_pendingScene, path))
	{
		delete m_pendingScene;
		m_pendingScene = nullptr;
		return false;
	}

	m_importing = true;
	m_importComplete = false;
	m_importProgress = 0.0f;
	return true;
}

void updateSceneLoad()
{
	if (!m_importing)
		return;

	if (!CSceneImporter::updateLoadScene())
	{
		m_importProgress = CSceneImporter::getTotalObjects() > 0 ?
			CSceneImporter::getLoadingPercent() : 0.0f;
		return;
	}

	m_importing = false;
	m_importProgress = 1.0f;
	m_pendingScene->updateAddRemoveObject();
	m_pendingScene->updateIndexSearchObject();
	m_importComplete = true;
}
```

Handle a failed `beginSceneLoad()` with an error state in your loading UI. `beginImportScene()` returning `false` detects failure to open the initial XML reader; it is not comprehensive content validation. The current implementation also reopens the XML without a separate failure check, so keep its backing files/archive available throughout import. `updateLoadScene()` returning `true` means deserialization finished, not that every referenced resource loaded correctly. Check logs and validate required objects/components afterward.

`getLoadingPercent()` reports an object-count ratio, not elapsed time, bytes, particle preload progress, or total application readiness. Guard empty scenes as above. Only call `updateLoadScene()` while an import is active; it is not an idle polling API.

## 3. Finish the hierarchy before querying

After import, `scene->updateAddRemoveObject()` recursively commits pending child additions/removals in every zone. `scene->updateIndexSearchObject()` rebuilds the lookup index. Import completion already refreshes the index; an explicit refresh after committing hierarchy changes also covers application additions and later renames.

Do not assume zone zero always exists, or that it is the only zone:

```cpp
for (int i = 0; i < scene->getZoneCount(); ++i)
{
	CZone* zone = scene->getZone(i);
	CGameObject* spawn = zone->searchObject(L"Spawn");
	if (spawn)
	{
		core::vector3df localSpawn = spawn->getPosition();
		// Consume the marker according to the application's transform convention.
	}
}
```

A marker name is an authoring convention. Check missing markers, duplicate names, and expected component types. `getPosition()` is local position; use the appropriate world transform when the marker is parented.

## 4. Find objects by layer

Use layer bits as authoring tags for runtime setup. Coordinate editor layer definitions in `Assets/BuiltIn/Settings/ObjectLayer.xml` with the runtime masks. These assignments are examples, not engine-reserved meanings:

```cpp
const u32 GroundLayer = 1u << 7;
const u32 WaterLayer = 1u << 8;

ArrayGameObject groundObjects;
zone->searchObjectByCullingLayer(groundObjects, GroundLayer);
for (CGameObject* object : groundObjects)
{
	CRenderMesh* mesh = object->getComponent<CRenderMesh>();
	if (!mesh)
		continue;

	mesh->setShadowCasting(false);
	mesh->changeShaderForAllMaterials(
		"BuiltIn/Shader/PBR/Forward/PBRLightmapShadow.xml");
}
```

Choose a shader compatible with your render pipeline, mesh attributes, textures, and lightmap setup. Receiving shadows and casting shadows are separate decisions; disabling casting does not itself disable receiving.

`searchObjectByCullingLayer()` recursively visits descendants, excluding the queried container itself, and accepts any overlap: `(object->getCullingLayer() & mask) != 0`. It appends to the result array without clearing it and returns the resulting array size. Use a fresh array or clear it before a new query. It does not filter by visibility or require a render component, and a matching container does not imply its children have the same layer.

Layers also control camera culling. `setCullingLayer(mask)` replaces all bits; `setCullingLayerOnOff(bit, true)` adds a bit while preserving others. Containers offer `setCullingLayerForChild(mask)` and `setCullingOnOffForChild(bit, on)` for descendants, excluding the container itself. Configure camera masks accordingly; see [Camera.md](Camera.md).

## 5. Change materials and initialize lightmaps

To change only selected materials, inspect each slot rather than replacing every shader:

```cpp
for (int i = 0; i < mesh->getMaterialCount(); ++i)
{
	CMaterial* material = mesh->getMaterial(i);
	if (!material || !material->getShader())
		continue;

	const std::string& name = material->getShader()->getName();
	if (name.find("CutOff") != std::string::npos)
		continue;

	material->changeShader("Materials/Shaders/Environment.xml");
	// Set uniforms/textures required by this application-owned shader here.
}
```

Use `std::string::npos` for substring checks. Preserve alpha-test, skinning, transparency, and other material-specific behavior when selecting replacements.

To load a replacement `.mat` and clone its materials for this renderer:

```cpp
std::vector<std::string> textureFolders;
ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
	"Materials/environment.mat", true, textureFolders);
if (!materials.empty())
	mesh->initMaterial(materials, true);
```

`changeShaderForAllMaterials()` clones the renderer materials before changing their shaders. Material assignment uses mesh/material naming conventions; verify that the authored names match. `CRenderMesh::initMaterial()` defaults `cloneMaterial` to `false`. Mutating a shared material can affect other renderers, so request clones when independent overrides are required. Keep material/texture packages alive while their users exist.

For standard baked lighting, apply the component's lightmap shader setup after import, before any intentional final shader overrides:

```cpp
for (CLightmap* lightmap : zone->getComponentsInChild<CLightmap>(true))
	lightmap->changeLightmapShader();
```

`getComponentsInChild<T>(false)` searches descendants; `true` additionally includes the queried container's own component. The boolean does not mean recursion or visibility.

For a custom lightmap shader, inspect `CLightmap::getLightDataExternal()`. When an entry is a texture-array descriptor, load it with `CTextureArraySerializable`, obtain its files with `getListFiles()`, and resolve the array through `CTextureManager::getTextureArray(files)`. Bind it through `material->setUniformTexture(...)` using the uniform declared by your shader. Validate descriptor count/content and texture creation; do not treat every lightmap as one ordinary 2D texture. Relevant headers are `Serializable/CTextureArraySerializable.h` and `TextureManager/CTextureManager.h`.

## 6. Add runtime components to tagged objects

Components added in code can adapt authored scene objects to runtime behavior. Here is an engine-only example enabling occlusion queries for water-tagged render meshes:

```cpp
ArrayGameObject waterObjects;
zone->searchObjectByCullingLayer(waterObjects, WaterLayer);
for (CGameObject* object : waterObjects)
{
	if (!object->getComponent<CRenderMesh>())
		continue;

	COcclusionQuery* query = object->getComponent<COcclusionQuery>();
	if (!query)
		query = object->addComponent<COcclusionQuery>();
	query->setEnable(true);
}
```

Apply the same get-or-add pattern for an application-defined water, trigger, or interaction component. Configure its renderer/query/camera dependencies before use. Such behavior belongs to the application; layer tagging does not automatically create water rendering or physics. Reflection also requires a camera, render target, render pass, and matching material bindings; see [Camera.md](Camera.md).

Import calls `startComponent()` for serialized objects. For runtime additions, follow the component's `initComponent()`/`startComponent()` contract and configure it explicitly; do not restart every component on an imported object as a generic initialization step.

## 7. Preload particle instances during loading

Create particle components and load `.particle` files before the destination scene becomes active. Retain the components for reuse instead of reconstructing them at the moment an effect is requested. A pool is an application policy built from engine components, not a required engine singleton.

The following loading-controller method creates one instance per call. Call it across loading frames using paths/counts from an application preload list:

```cpp
bool preloadParticle(CContainerObject* container, const char* path)
{
	CGameObject* object = container->createEmptyObject();
	Particle::CParticleComponent* particle =
		object->addComponent<Particle::CParticleComponent>();
	particle->setSourcePath(path);
	if (!particle->load())
	{
		object->remove();
		return false;
	}

	particle->Stop();
	particle->clearParticles();
	m_particleInstances.push_back(particle);
	return true;
}
```

`m_particleInstances` is an application-owned `std::vector<Particle::CParticleComponent*>`. Create its parent with `zone->createContainerObject()`. Commit new objects before normal scene updates/queries. Decide whether a failed required effect stops loading or an optional effect is skipped, and report progress separately from scene import.

`setSourcePath()` only records the path; `load()` reads and builds the effect. Serialized particle components are loaded by their `startComponent()`, so do not reload those indiscriminately. `Stop()` stops emission but does not remove all finite-lived particles; use `clearParticles()` when a completely empty preloaded instance is required. Loading does not guarantee all first-render shader/driver costs or peak particle-buffer allocations have been exercised.

For an independently positioned effect, select a free instance, set its object's transform, and call `Play()`. Keep transforms consistent with the effect's local/world-space configuration. `IsPlaying()` tests live particles; delayed emitters may need additional application busy state rather than treating an empty particle count as permission to reuse.

For compatible world-space burst effects, one component may emit at multiple locations using `setGroupTransform(world)` followed by `Play()`. Queue requests and allow birth to finish before changing the group transform/replaying; repeated calls in one frame can overwrite pending emission. Existing particles must remain at their old positions. Continuous Flow, delayed emitters, or particles following a moved transform need independent instances or a suitable lifecycle. Keep gameplay/sound timing independent of queued visual bursts. A shared component does not guarantee one draw call across groups and materials.

If a custom system consumes `particle->getGroup(0)`, first check `getNumOfGroup() > 0`. Register/configure that system after preload; clear retained component/group pointers before destroying their scene or reloading the effect.

## 8. Activate the prepared scene

A useful loading sequence is:

1. Present a loading frame and prepare resource packages/subsystems.
2. Begin import; advance it over loading updates.
3. Commit hierarchy changes and refresh lookup indexes.
4. Validate zones, markers, cameras, and required components.
5. Initialize baked lighting, then apply final layer/material/component overrides.
6. Preload effects and other runtime resources in bounded steps.
7. Commit new objects, refresh indexes if needed, and update the scene once to initialize transforms/systems.
8. Give the application's renderer the new scene and its world/UI cameras, then retire the loading scene.

`CSceneImporter` does not select a global active scene, choose a camera, construct a render pipeline, or transfer ownership. Those responsibilities belong to the host application. The first update can run simulation, so gate application behavior until preparation is complete.

Detach old scene consumers, callbacks, cameras, systems, and cached pointers before deleting the old scene. Destroy users before releasing their mesh/material/texture resources. Do not indiscriminately release global resource caches if the loading or destination scene still uses them. See [FirstInit.md](FirstInit.md) for package transitions.

## Engine source references

- [CSceneImporter.h](../Projects/Skylicht/Engine/Scene/CSceneImporter.h) and [implementation](../Projects/Skylicht/Engine/Scene/CSceneImporter.cpp): import phases, batching, progress, component startup, shared state.
- [CScene.cpp](../Projects/Skylicht/Engine/Scene/CScene.cpp): hierarchy commits, search indexes, scene updates.
- [CContainerObject.h](../Projects/Skylicht/Engine/GameObject/CContainerObject.h) and [implementation](../Projects/Skylicht/Engine/GameObject/CContainerObject.cpp): recursive component/layer queries and descendant layer updates.
- [CRenderMesh.cpp](../Projects/Skylicht/Engine/RenderMesh/CRenderMesh.cpp): material assignment, cloning, shader replacement.
- [CLightmap.cpp](../Projects/Skylicht/Engine/Lightmap/CLightmap.cpp): baked-lighting setup.
- [CParticleComponent.h](../Projects/Skylicht/Components/ParticleSystem/CParticleComponent.h) and [implementation](../Projects/Skylicht/Components/ParticleSystem/CParticleComponent.cpp): particle loading, playback, groups, transforms, cleanup.
