# Custom render systems: materials, buffers, and instanced drawing

This guide follows [Architecture.md](Architecture.md), [SceneLoading.md](SceneLoading.md), and [Camera.md](Camera.md). It builds a custom transparent renderer from Skylicht APIs: load a material, initialize a quad and an instance buffer, register an `IRenderSystem`, and submit the draw through the scene's render pipeline. The example is independent of any game, gameplay system, or private asset bundle.

## 1. Separate simulation from rendering

An application component or simulation system produces effect positions, colors, and sizes. The render system translates those values into GPU instance records and draws them. `Particle::CParticleInstancing` is useful even without a `.particle` file or particle emitters: it provides a mesh buffer with a second vertex stream configured for hardware instancing.

| Method | Responsibility |
| --- | --- |
| Constructor | Select the pipeline/pass and initialize owned members. |
| `init(manager)` | Called once when a newly created system is registered; resolve dependencies or allocate resources. |
| `beginQuery(manager)` | Clear transient query results if the renderer collects entities. |
| `onQuery(manager, entities, count)` | Collect eligible entity data, or leave empty when input is supplied explicitly. |
| `update(manager)` | Build instance data and prepare information for the current render view. |
| `render(manager)` | Submit opaque/primary drawing, or leave empty for a transparent-only renderer. |
| `renderTransparent(manager)` | Submit transparent drawing. |
| Destructor | Release resources owned by this system. |

Render systems are queried/updated by `cullingAndRender()` or `updateRenderer()`, rather than the normal non-render-system update loop. Rendering several cameras/passes can therefore prepare the renderer several times per application frame. Keep damage, timers, emission scheduling, and other simulation changes in the simulation update. Prepare camera-dependent billboard state using the view associated with the draw.

`m_renderPass = Transparent` determines system sorting, not alpha blending, and does not automatically route drawing into the right method. The entity manager calls `render()` for compatible systems, then calls `renderTransparent()` separately. Leave `render()` empty when all your draws belong to the latter.

## 2. Material and shader contract

A `.mat` is a serialized material; a shader `.xml` describes shader stages, uniforms, and the base render state. These are two alternative ways to obtain a `CMaterial`:

```cpp
// Build a new material directly from an application-owned shader definition.
m_material = new CMaterial("BillboardEffect", "Shaders/BillboardEffect.xml");
```

```cpp
// Or load an authored .mat and take a private clone of the intended entry.
std::vector<std::string> textureFolders;
ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
	"Materials/BillboardEffect.mat", true, textureFolders);
if (!materials.empty())
	m_material = materials[0]->clone();
```

Choose the intended material by name when the file contains several entries. The manager owns its cached entries; the clone belongs to your renderer. Do not overwrite an existing owned material without releasing it first. In the examples below, `m_material` owns one reference, released with `drop()`.

Configure the material after creation:

```cpp
m_material->setBackfaceCulling(false);
m_material->setZWrite(false);
m_material->setZTest(video::ECFN_LESSEQUAL);

CTextureManager* textures = CTextureManager::getInstance();
m_material->setTexture(0, textures->getTexture("Textures/effect_alpha.png"));
m_material->setTexture(1, textures->getTexture("Textures/effect_noise.png"));

float intensity[] = { 2.0f, 2.0f, 2.0f, 1.0f };
m_material->setUniform4("uColorIntensity", intensity);
m_material->updateShaderParams();
```

The paths, texture slots, and `uColorIntensity` above are application-defined. Mount resources and initialize shader support before creating the renderer. Validate `getShader()` and required textures; a material pointer alone does not prove its shader/resources are available.

The shader must consume the actual vertex descriptor and instance layout described below. For billboard shaders using `CShaderParticle`, declare the corresponding `PARTICLE_VIEW_UP` and `PARTICLE_VIEW_LOOK` uniforms in the shader XML; a `MATERIAL_PARAM` uniform must match the material parameter name/index. Supply shader programs for the graphics backends your application supports.

Choose the shader XML's blend state intentionally, for example `TRANSPARENT_ADD_COLOR` for a compatible additive effect. The render-pass enum does not set blending. Keep depth testing for world effects that should be occluded; `ECFN_ALWAYS` intentionally draws through geometry. Disable depth writes for the usual transparent billboard, and preserve alpha-test/soft-particle/depth-texture requirements when designing a different effect.

## 3. Define the custom system

The following fragments form one application class. Include your normal `pch.h` first in its `.cpp`; use the application's usual namespace setup for `Skylicht`, Irrlicht core/video types, and engine helpers. No engine-source changes are required. Link the Components library for `CParticleInstancing`.

```cpp
#include "Entity/CEntityManager.h"
#include "Entity/IRenderSystem.h"
#include "ParticleSystem/Particles/CParticleInstancing.h"
#include "Material/CMaterialManager.h"
#include "TextureManager/CTextureManager.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"

class CBillboardEffectRenderer : public IRenderSystem
{
protected:
	Particle::CParticleInstancing* m_instancing;
	CMaterial* m_material;
	std::vector<Particle::SParticleInstance> m_instances;
	bool m_enabled;

	void initQuad();

public:
	CBillboardEffectRenderer();
	virtual ~CBillboardEffectRenderer();

	void init(CEntityManager* manager) override;
	void beginQuery(CEntityManager* manager) override {}
	void onQuery(CEntityManager* manager, CEntity** entities, int count) override {}
	void update(CEntityManager* manager) override;
	void render(CEntityManager* manager) override {}
	void renderTransparent(CEntityManager* manager) override;

	void setEnabled(bool enabled) { m_enabled = enabled; }
	void setInstances(const std::vector<Particle::SParticleInstance>& instances)
	{
		m_instances = instances;
	}
};

CBillboardEffectRenderer::CBillboardEffectRenderer() :
	m_instancing(nullptr),
	m_material(nullptr),
	m_enabled(false)
{
	m_pipelineType = IRenderPipeline::Forwarder;
	m_renderPass = Transparent;
}

void CBillboardEffectRenderer::init(CEntityManager* manager)
{
	m_instancing = new Particle::CParticleInstancing();
	initQuad();

	m_material = new CMaterial("BillboardEffect", "Shaders/BillboardEffect.xml");
	m_material->setBackfaceCulling(false);
	m_material->setZWrite(false);
	m_material->setZTest(video::ECFN_LESSEQUAL);

	CTextureManager* textures = CTextureManager::getInstance();
	m_material->setTexture(0, textures->getTexture("Textures/effect_alpha.png"));
	m_material->setTexture(1, textures->getTexture("Textures/effect_noise.png"));
	float intensity[] = { 2.0f, 2.0f, 2.0f, 1.0f };
	m_material->setUniform4("uColorIntensity", intensity);
	m_material->updateShaderParams();
}

CBillboardEffectRenderer::~CBillboardEffectRenderer()
{
	if (m_material)
		m_material->drop();
	delete m_instancing;
}
```

This renderer takes copied instance values, so it retains no entity/data pointers. It starts disabled; the application enables it only after checking the required shader and textures. Add a readiness/error API if the application needs to expose resource failures in its loading UI.

## 4. Initialize the base mesh buffer once

`CParticleInstancing` creates a standard `S3DVertex` base mesh with 16-bit indices and attaches an `SParticleInstance` stream. It does not create the quad geometry for you. Fill four vertices and six indices:

```cpp
void CBillboardEffectRenderer::initQuad()
{
	IMeshBuffer* mesh = m_instancing->getMeshBuffer();
	IVertexBuffer* vertices = mesh->getVertexBuffer();
	IIndexBuffer* indices = mesh->getIndexBuffer();
	vertices->set_used(4);
	indices->set_used(6);

	// CParticleInstancing currently constructs an EIT_16BIT index buffer.
	u16* index = reinterpret_cast<u16*>(indices->getIndices());
	const u16 quad[] = { 0, 1, 2, 0, 2, 3 };
	for (int i = 0; i < 6; ++i)
		index[i] = quad[i];

	video::S3DVertex* vertex =
		reinterpret_cast<video::S3DVertex*>(vertices->getVertices());
	const core::vector3df positions[] = {
		{ -1.0f,  1.0f, 0.0f }, { 1.0f,  1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f }, { -1.0f, -1.0f, 0.0f }
	};
	const core::vector2df uvs[] = {
		{ 0.0f, 0.0f }, { 1.0f, 0.0f },
		{ 1.0f, 1.0f }, { 0.0f, 1.0f }
	};
	for (int i = 0; i < 4; ++i)
	{
		vertex[i].Pos = positions[i];
		vertex[i].Normal.set(0.0f, 0.0f, 1.0f);
		vertex[i].Color = video::SColor(255, 255, 255, 255);
		vertex[i].TCoords = uvs[i];
	}
	mesh->recalculateBoundingBox();
	mesh->setDirty();
}
```

If adapting this helper to another buffer, inspect its index type and write `u16` or `u32` accordingly. Never cast arbitrary vertex storage to `S3DVertex` unless its descriptor/stride matches. Mark buffers dirty after CPU writes so the driver uploads the changed data.

The quad is centered at the origin with coordinates from -1 to +1. Its final world dimensions depend on how the vertex shader applies `Size`; document whether your shader treats size as full extent or half extent. A base-mesh bounding box does not enclose all instances; implement suitable aggregate/per-instance culling if needed.

## 5. Fill the instance buffer

There are two streams: four base vertices shared by all instances, and one `SParticleInstance` record per effect. The instance stream uses a per-instance step rate configured by `CParticleInstancing`; use `drawMeshBuffer()` rather than manually drawing the quad once per effect.

| Instance field | Typical shader use |
| --- | --- |
| `Pos` (`SVec4`) | World position; explicitly initialize W. |
| `Color` (`SColor`) | Per-instance tint and alpha. |
| `UVScale`, `UVOffset` | Texture atlas selection or animated UV offsets. |
| `Size` (`SVec4`) | Billboard dimensions/scale. |
| `Rotation` (`SVec4`) | Rotation or application-defined orientation data. |
| `Velocity` (`SVec4`) | Direction for a velocity-aligned billboard. |

The exact meaning is determined by your shader, not by the field names alone. Match the descriptor attributes `inParticlePosition`, `inParticleColor`, `inParticleUVScale`, `inParticleUVOffset`, `inParticleSize`, `inParticleRotation`, and `inParticleVelocity`. Preserve the engine's packing and color interpretation across GLSL/HLSL.

```cpp
void CBillboardEffectRenderer::update(CEntityManager* manager)
{
	auto* buffer = m_instancing->getInstanceBuffer();
	const u32 count = m_enabled ? static_cast<u32>(m_instances.size()) : 0;
	buffer->set_used(count);
	if (count > 0)
	{
		auto* output = reinterpret_cast<Particle::SParticleInstance*>(
			buffer->getVertices());
		for (u32 i = 0; i < count; ++i)
			output[i] = m_instances[i];
	}
	buffer->setDirty();
}
```

Resize before obtaining the writable pointer: `set_used()` can reallocate storage. Clear the count when there is no input so old effects are not drawn. For frequently reused effects, retain the allocation and avoid reconstructing the mesh/material each frame.

Some ANGLE/GLES configurations require a workaround for a single instance. If you confirm that issue on a supported backend, allocate two records when the logical count is one, copy the real record, and initialize the entire second record with zero size and alpha. The physical buffer count becomes two. This is a backend workaround, not a general requirement; do not leave the dummy record's rotation/velocity or other fields uninitialized.

## 6. Bind and draw in the transparent callback

```cpp
void CBillboardEffectRenderer::renderTransparent(CEntityManager* manager)
{
	if (!m_enabled || !m_material || !m_material->getShader())
		return;
	if (m_instancing->getInstanceBuffer()->getVertexCount() == 0)
		return;

	IVideoDriver* driver = getVideoDriver();
	IMeshBuffer* mesh = m_instancing->getMeshBuffer();
	if (mesh->getIndexBuffer()->getIndexCount() == 0)
		return;

	// Billboard axes must match this draw's camera view.
	core::matrix4 inverseView;
	driver->getTransform(video::ETS_VIEW).getInversePrimitive(inverseView);
	core::vector3df up(inverseView[4], inverseView[5], inverseView[6]);
	core::vector3df look(inverseView[8], inverseView[9], inverseView[10]);
	up.normalize();
	look.normalize();
	CShaderParticle::setViewUp(up);
	CShaderParticle::setViewLook(look);

	// Instance positions are already in world space.
	driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
	video::SMaterial& drawMaterial = mesh->getMaterial();
	m_material->updateTexture(drawMaterial);
	CShaderMaterial::setMaterial(m_material);
	driver->setMaterial(drawMaterial);
	driver->drawMeshBuffer(mesh);
}
```

`updateTexture()` transfers textures, depth/culling settings, and the shader material type into the low-level `SMaterial`. `CShaderMaterial::setMaterial()` selects the high-level material used by shader callbacks for its uniform parameters. `driver->setMaterial()` binds the low-level draw state, then `drawMeshBuffer()` submits the indexed instanced mesh. These calls have different jobs; assigning textures alone is insufficient.

`getInversePrimitive()` assumes a rigid view transform; use an appropriate inverse/basis calculation if your camera setup violates that assumption. Set shared shader callback state immediately before your own draw so another renderer cannot leave the wrong material or billboard axes active.

One compatible material and mesh can batch many effects into one submission for this callback. Multiple materials, texture sets, cameras, or pipeline passes require more submissions. Transparent instances are not automatically sorted back-to-front by this renderer; add per-view sorting for ordinary alpha blending, or choose an effect/blend mode where that ordering is acceptable.

## 7. Register the system and provide input

Register during scene setup on the engine thread, before rendering begins:

```cpp
CEntityManager* manager = scene->getEntityManager();
CBillboardEffectRenderer* renderer =
	manager->addRenderSystem<CBillboardEffectRenderer>();

std::vector<Particle::SParticleInstance> instances;
Particle::SParticleInstance instance{};
instance.Pos.X = 0.0f;
instance.Pos.Y = 2.0f;
instance.Pos.Z = 0.0f;
instance.Pos.W = 1.0f;
instance.Color = video::SColor(200, 255, 160, 60);
instance.UVScale.set(1.0f, 1.0f);
instance.UVOffset.set(0.0f, 0.0f);
instance.Size.X = 1.0f;
instance.Size.Y = 1.5f;
instance.Size.Z = 1.0f;
instance.Size.W = 0.0f;
instance.Rotation.X = instance.Rotation.Y = 0.0f;
instance.Rotation.Z = instance.Rotation.W = 0.0f;
instance.Velocity.X = instance.Velocity.Z = instance.Velocity.W = 0.0f;
instance.Velocity.Y = 1.0f;
instances.push_back(instance);
renderer->setInstances(instances);
renderer->setEnabled(true);
```

Resource validation belongs before enabling the renderer. This fragment assumes the application-owned shader and textures described above are available; it is not a complete shader implementation.

`addRenderSystem<T>()` constructs a new system, calls `init(manager)`, and registers it for both system lookup and rendering. If a matching system already exists, it returns that instance without calling `init()` again. Dependencies resolved in `init()` must therefore already exist. Use `getSystem<T>()` or `getRenderSystem<T>()` to retrieve the registered renderer later.

A component can register it in `initComponent()` as well:

```cpp
void CEffectController::initComponent()
{
	m_gameObject->getEntityManager()->addRenderSystem<CBillboardEffectRenderer>();
}
```

`CEffectController` here is an application-defined component. Multiple components share the same registered renderer, so a system-level enable flag affects all of them. Per-object state belongs in per-object data or an aggregated instance list. Removing one controller does not automatically remove the shared system.

Update the copied input list from the application's simulation code before rendering. A renderer using entity data instead can collect matching data in `onQuery()` and rebuild its transient list each query. The manager supplies alive entities, not an automatically layer/frustum-filtered list for your custom effect; implement visibility, camera masks, and culling appropriate to the renderer. The explicit-input example draws its input in every compatible view unless you add filtering.

The default `IRenderSystem` pipeline type is `Forwarder`. A deferred application needs a compatible forward transparent stage or a renderer designed for the relevant pipeline. Use `Mix` only when the implementation really supports each pipeline that can invoke it. The host must still update the scene and render it through a pipeline with a camera; registration alone does not render anything.

Systems are sorted by render pass, then ascending sorting priority. To change priority after registration, call `setSortingPriority(...)` and `manager->notifyRendererOrderChanged()`. Sorting systems does not sort individual transparent instances.

## 8. Ownership and lifetime

The entity manager owns registered systems and deletes them during teardown. Do not `delete` a pointer returned by `addRenderSystem()`. To remove a system early, use `manager->removeSystem(renderer)` outside active system iteration and invalidate consumers of the pointer. In this checkout, `removeSystem()` does not mark the cached sorted lists dirty: also call `manager->notifySystemOrderChanged()` and `manager->notifyRendererOrderChanged()` before the next update/render so those lists are rebuilt.

The renderer owns `CParticleInstancing` through `new`/`delete`; that helper releases its mesh and instance-buffer references. Do not separately delete/drop the borrowed pointers returned by `getMeshBuffer()` and `getInstanceBuffer()`. Release a newly created or cloned `CMaterial` with `drop()`. Texture-manager results are borrowed; keep their resource package alive until the renderer no longer uses it.

Transient query lists can be discarded and rebuilt without dereferencing old entries. Separate retained targets, delayed requests, or cached entity-data pointers need removal cleanup before use after entity destruction. Prefer copied render values when the effect does not need live entity references. Clear application-held renderer pointers before destroying the scene, and destroy rendering users before releasing shader/texture packages.

## Engine source references

- [IRenderSystem.h](../Projects/Skylicht/Engine/Entity/IRenderSystem.h): pipeline type, passes, render callbacks, priority.
- [CEntityManager.h](../Projects/Skylicht/Engine/Entity/CEntityManager.h) and [implementation](../Projects/Skylicht/Engine/Entity/CEntityManager.cpp): registration, ownership, query/update flow, pipeline filtering, sorting.
- [CParticleInstancing.h](../Projects/Skylicht/Components/ParticleSystem/Particles/CParticleInstancing.h) and [implementation](../Projects/Skylicht/Components/ParticleSystem/Particles/CParticleInstancing.cpp): instance layout, descriptors, stream rate, buffer ownership.
- [CMaterial.cpp](../Projects/Skylicht/Engine/Material/CMaterial.cpp): shader initialization, material cloning, uniform parameters, low-level material state.
- [CShaderMaterial.h](../Projects/Skylicht/Engine/Material/Shader/ShaderCallback/CShaderMaterial.h) and [CShaderParticle.h](../Projects/Skylicht/Engine/Material/Shader/ShaderCallback/CShaderParticle.h): callback inputs selected before drawing.
