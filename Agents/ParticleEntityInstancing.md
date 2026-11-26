# Particle instancing: entity attachments and lightweight objects

This guide follows [SceneLoading.md](SceneLoading.md) and [CustomRenderSystem.md](CustomRenderSystem.md). It uses one shared particle group to display many entity-attached icons: create one particle per owner, synchronize its position and atlas frame, and remove it when the icon or owner disappears. It also covers particle-only simulated objects, including mesh instances with custom flight and collision logic, without an entity per object. Examples use engine APIs and application-defined classes only.

## 1. Share the renderer, not the ownership

Create one `Particle::CParticleComponent` with a quad-instancing group for compatible icons. Each entity stores its icon state and the index of its particle in that group. A normal `IEntitySystem` synchronizes this state; the engine particle systems prepare buffers and render the group. No custom `IRenderSystem` or GameObject per icon is necessary.

This is a logical attachment, not scene parenting: the particle does not automatically follow the entity. Your synchronization system writes its position each update. A shared atlas/material lets many icons use the same instanced batch, subject to renderer, backend, camera, and pass behavior; it does not guarantee a single draw across an entire frame.

## 2. Prepare a manually controlled particle group

Author an application asset such as `Particles/entity_icons.particle` with:

- A quad instancing renderer, an icon atlas texture, and its atlas column/row counts.
- At least one emitter (emitter index zero is used below), with zero force and zero automatic Flow.
- Positive `LifeMin`/`LifeMax`, so manual creation initializes models and invokes birth callbacks.
- No gravity, motion, or animated models that compete with application-controlled position/frame values.
- Optimized removal enabled (`group->Optimized = true`) for the swap-based bookkeeping below.

Load it during scene preparation:

```cpp
CGameObject* object = zone->createEmptyObject();
auto* particles = object->addComponent<Particle::CParticleComponent>();
particles->setSourcePath("Particles/entity_icons.particle");
if (!particles->load() || particles->getNumOfGroup() == 0)
{
	// Enter the application's loading error path; do not bind a missing group.
	return;
}
particles->Stop();
particles->clearParticles();
Particle::CGroup* group = particles->getGroup(0);
group->Optimized = true;
```

Stop automatic emission before attaching owners. `Stop()` also marks immortal particles for removal, so do not call it on an active shared icon group merely to hide one icon. `clearParticles()` is a group-wide operation, not per-owner removal.

Keep this host object at an identity transform for the world-position convention below. Keep the particle component enabled so group updates can process requested removals. Ensure its culling layer is visible to the intended camera. With a shared host, camera masks apply to the host; per-owner visibility is an application decision.

## 3. Store per-entity state

An application-defined data type can contain:

```cpp
class CEntityIconData : public IEntityData
{
public:
	bool Visible = false;
	int ParticleId = -1;
	int Frame = 0;
	core::vector3df Offset = core::vector3df(0.0f, 2.0f, 0.0f);
};
DECLARE_PRIVATE_DATA_TYPE_INDEX(CEntityIconData);
```

Place `IMPLEMENT_DATA_TYPE_INDEX(CEntityIconData);` in its `.cpp`. Attach it with `entity->addData<CEntityIconData>()` and retrieve it through `GET_ENTITY_DATA(entity, CEntityIconData)` as in other engine data types. The entity owns this data; the particle group owns the particle. `ParticleId == -1` means the owner has no attached particle.

An optional timer or atlas-animation range also belongs here. Keep timing in the ordinary entity system, once per simulation update, rather than in render callbacks that can execute for several cameras. For looping frames, define whether the end frame is inclusive/exclusive and retain leftover time when stepping animations. Set `p.Params[Particle::FrameIndex]` to a valid atlas cell.

Use these engine headers where needed:

```cpp
#include "Entity/IEntityData.h"
#include "Entity/IEntitySystem.h"
#include "Entity/CEntityManager.h"
#include "Entity/CEntityGroup.h"
#include "Transform/CWorldTransformData.h"
#include "ParticleSystem/CParticleComponent.h"
#include "ParticleSystem/CParticleGroupSystem.h"
#include "ParticleSystem/Particles/CGroup.h"
```

## 4. Synchronize owners in a normal entity system

Create an application `CEntityIconSystem : public IEntitySystem, public Particle::IParticleCallback`. Store a borrowed `Particle::CGroup* m_particles = nullptr` and a parallel `std::vector<CEntityIconData*> m_owners`. The callback interface is declared in `CGroup.h`.

Register it with `manager->addSystem<CEntityIconSystem>()`. `init()` can initialize dependencies; `beginQuery()` can find/create a `CEntityGroup` containing `CEntityIconData` and `CWorldTransformData`. In `onQuery()`, rebuild transient icon/transform lists from that valid group. Do not interpret the `entities` argument as an already filtered list of icon owners.

For a simple implementation, process these query results directly in `update()`:

1. If the group is not bound, return before any access to its particles.
2. Resolve the owner's icon data and transform; check required data is present.
3. If the icon is disabled or owner visibility fails your policy, request removal of its existing particle.
4. Otherwise calculate the anchor position, create a missing particle, and update its position/frame.

Use `transform->getWorldPosition() + icon.Offset` after the world-transform system has updated. `Offset` is a world-axis offset in this convention. For a rotated/scaled local anchor, transform that local point by the owner's world matrix instead. `getRelativePosition()` only works as a world position when the owner's parent transform is identity; do not use it indiscriminately for nested objects.

### Create a particle and link it to its owner

The following is a method body fragment on `CEntityIconSystem`; `icon` and `position` have been validated by the query/update loop:

```cpp
int id = m_particles->addParticleVelocity(
	0, position, core::vector3df(0.0f, 0.0f, 0.0f));
if (id < 0)
	return;

// Creation invokes OnParticleBorn synchronously for a valid positive lifetime.
// Resize defensively as well, before indexing the application owner table.
if (m_owners.size() <= static_cast<size_t>(id))
	m_owners.resize(id + 1, nullptr);

icon.ParticleId = id;
m_owners[id] = &icon;
Particle::CParticle& particle = m_particles->getParticles()[id];
particle.Immortal = true;
particle.Position = position;
particle.LastPosition = position;
particle.Params[Particle::FrameIndex] = static_cast<float>(icon.Frame);
```

The emitter index is not an entity ID. An invalid emitter index returns `-1`; check before indexing. Positive authored lifetime is important: the current manual-add path only initializes models/birth callbacks when `LifeTime > 0`.

`Immortal` prevents normal lifetime expiration; it does not make the particle independent of its owner or exempt it from explicit death. Keep the data/group association valid until your removal policy detaches it.

### Follow the entity

For an existing particle, validate the index and reverse association first:

```cpp
const int id = icon.ParticleId;
if (id >= 0 &&
	id < static_cast<int>(m_particles->getParticles().size()) &&
	id < static_cast<int>(m_owners.size()) && m_owners[id] == &icon)
{
	Particle::CParticle& particle = m_particles->getParticles()[id];
	particle.Position = position;
	particle.LastPosition = position;
	particle.Params[Particle::FrameIndex] = static_cast<float>(icon.Frame);
}
```

Update both position fields for an anchored icon so the previous position does not retain an obsolete location. Do not keep a `CParticle*` or reference between updates: adding particles can reallocate the array, and removing particles can swap slots.

### Order synchronization before particle simulation

Create the particle component first, then register/configure the attachment system. Ensure its system order is after world transforms and before `Particle::CParticleGroupSystem`, so the group sees the current positions, removes dead particles, and builds bounds before rendering. Registration order alone may put the new icon system after the already registered particle system.

```cpp
auto* particleSystem = manager->getSystem<Particle::CParticleGroupSystem>();
if (particleSystem)
{
	iconSystem->setSystemOrder(particleSystem->getSystemOrder());
	particleSystem->setSystemOrder(particleSystem->getSystemOrder() + 1);
	manager->notifySystemOrderChanged();
}
```

This example assumes the selected order is after world transforms and does not conflict with other application systems; inspect those dependencies in your application. Explicitly notify after changing order. If another simulation system supplies the anchor, run synchronization after that producer as well. An alternative is final position synchronization in `OnParticleUpdate()`, which runs after particle simulation and before dead-particle removal/bounds calculation, provided owner references remain valid there. Do not create/reallocate particles while using the callback's borrowed particle-array pointer.

## 5. Particle indices move: maintain the reverse table

`ParticleId` is an array slot, not a permanent handle. In optimized removal, the engine calls `OnSwapParticleData()` for the dead slot and the last slot, swaps their particle contents, calls `OnParticleDead()` for the last slot, then shrinks the particle array.

The parallel owner table must follow the same swap. These are `CEntityIconSystem` callback implementations:

```cpp
void CEntityIconSystem::OnParticleBorn(Particle::CParticle& particle)
{
	if (m_owners.size() <= particle.Index)
		m_owners.resize(particle.Index + 1, nullptr);
	m_owners[particle.Index] = nullptr;
}

void CEntityIconSystem::OnSwapParticleData(
	Particle::CParticle& a, Particle::CParticle& b)
{
	CEntityIconData* owner = m_owners[a.Index];
	m_owners[a.Index] = m_owners[b.Index];
	m_owners[b.Index] = owner;
	if (m_owners[a.Index])
		m_owners[a.Index]->ParticleId = static_cast<int>(a.Index);
	if (m_owners[b.Index])
		m_owners[b.Index]->ParticleId = static_cast<int>(b.Index);
}

void CEntityIconSystem::OnParticleDead(Particle::CParticle& particle)
{
	if (particle.Index >= m_owners.size())
		return;
	CEntityIconData* owner = m_owners[particle.Index];
	if (owner)
		owner->ParticleId = -1;
	m_owners[particle.Index] = nullptr;
}
```

This version leaves null slots in the application table; reuse/resize on later births and clear the table when unbinding the group. It works with the current `clearParticles()` callback sequence too: that operation reports each particle in ascending order without swap-removal. A callback that always assumes the dead owner is the last table entry must instead explicitly constrain/support bulk clearing.

Do not use this swap-based recipe with `Optimized = false`: that branch erases from the middle without the same swap callbacks. Support its shifting semantics separately or keep optimized removal enabled. Use a dedicated group; unrelated particle writers and callbacks must respect the same ownership policy.

## 6. Hide or detach an icon

Request removal by setting `Life = -1.0f`; the next group update removes it even when `Immortal` is true. Do not erase the engine particle array manually.

A useful application helper both requests death and detaches the owner immediately:

```cpp
void CEntityIconSystem::detach(CEntityIconData& icon)
{
	const int id = icon.ParticleId;
	if (id >= 0 && id < static_cast<int>(m_owners.size()) &&
		m_owners[id] == &icon)
	{
		if (m_particles &&
			id < static_cast<int>(m_particles->getParticles().size()))
			m_particles->getParticles()[id].Life = -1.0f;
		m_owners[id] = nullptr;
	}
	icon.ParticleId = -1;
}
```

Set `icon.Visible = false` as well when disabling the feature; otherwise the next synchronization can recreate its particle. The death callback must accept null owners because an entity may already have detached. Avoid reusing a dead-but-not-yet-removed slot; let the group compact it normally. If the particle update is paused, requested death is pending until updates resume.

## 7. Handle entity destruction and scene teardown

The reverse table retains entity-data pointers beyond each query, so rebuilding the query list alone is insufficient. Pick a concrete removal strategy:

- Implement `IEntityManagerCallback` on the attachment system, register/unregister with the manager, and handle both single and batch entity removal. The overloads are `onEntityRemoved(CEntity*)` and `onEntityRemoved(CEntity**, int)`; registration is `registerCallback(this)` paired with `unRegisterCallback(this)`. At removal notification, data still exists: retrieve `CEntityIconData` and call `detach()` before the removal flush frees it. Also handle explicit icon-data removal if your application supports removing data without removing its entity.
- Alternatively, give the icon data an application-defined removal listener. Its destructor calls the listener with `this`, and the system calls `detach()` while that data's own fields still exist. Clear listener pointers before the listener/system is destroyed. Such a listener is an application interface, not an engine `IDataRemoveSystem` API.

A data destructor must not dereference a cached transform, gameplay data, or sibling component; entity data is deleted by data index, so those objects may already be gone. It may notify using its own still-valid icon fields. Keep particle-group access behind a valid borrowed group pointer and clear that pointer on group destruction.

Provide explicit bind/unbind methods. Bind an empty group and call `addCallback(this)` before creating attached particles. Before switching groups or destroying the system:

1. Detach all owners while their data is still valid and clear any data removal-listener pointers.
2. Remove the particle callback with `removeCallback(this)` if the old group is still alive.
3. Clear the reverse table and reset the group pointer.
4. Unregister manager removal callbacks before destroying the system.

Implement `OnGroupDestroy()` to invalidate all still-live owners' `ParticleId`, clear the reverse table, and set the group pointer to null. Do not perform group operations or mutate its callback list from that destruction callback; the UserData cleanup variant in section 9 only visits the still-existing particle storage. Group destruction does not guarantee an individual death callback for each particle. Reloading a `.particle` file can destroy its old groups, so unbind/rebind around reload too.

These rules allow either destruction order: owner first requests death and nulls its reverse entry; group first invalidates owners' indices; system first unregisters and clears its relationships. The host application must arrange cleanup while the participants are still valid. Entity-manager teardown releases entity data before systems, which makes a surviving removal listener useful, but does not replace explicit cleanup when a system is removed early.

## 8. Validate the behavior

Useful integration checks are: move a parented entity; show/hide/show its icon; remove a middle particle while other icons remain; destroy the owner before particle cleanup; clear/reload the group; destroy the group before the system; and render several cameras. Confirm surviving icons retain the right owner/frame, no ghost icon remains, and no callback accesses freed data.

The particle renderer handles the GPU instance-buffer work described in [CustomRenderSystem.md](CustomRenderSystem.md). This technique supplies particle state and lifetime rather than manually maintaining another instancing mesh.

## 9. Use particles as lightweight simulated objects

The attachment approach above still has an entity for each icon owner. A second approach makes each particle the simulated object itself: a projectile, flying shard, or falling mesh. Store position, previous position, velocity, and lifetime in `CParticle`, and store application state alongside it. There is no entity, GameObject, transform component, or render component per projectile.

A shared host GameObject/entity with `CParticleComponent` still integrates the group into the scene. A controller can be one component or application-owned callback object for the whole group. This reduces per-object allocation, component queries, and lifecycle work; CPU simulation/collision and GPU rendering still have costs. Use entities when objects require independent component composition, hierarchy, or physics bodies. Particle collision is application logic, not an automatically created physics collider.

### Choose quad or mesh instancing

For sprites, retain a quad renderer. For actual 3D objects, author the `.particle` with a mesh renderer, or create a group programmatically:

```cpp
#include "ParticleSystem/Particles/Renderers/CMeshParticleRenderer.h"

// particles is one shared, initialized CParticleComponent.
Particle::CFactory* factory = particles->getParticleFactory();
Particle::CGroup* group = particles->createParticleGroup();
group->LifeMin = group->LifeMax = 5.0f;
group->Optimized = true;

// Manual creation still needs emitter zero, even with no automatic emission.
auto* emitter = group->addEmitter(factory->createStraightEmitter(core::vector3df(0.0f, 1.0f, 0.0f)));
emitter->setZone(factory->createPointZone(core::vector3df()));
emitter->setFlow(0.0f);
emitter->setTank(0);
emitter->setForce(0.0f, 0.0f);

auto* renderer = factory->createMeshParticleRenderer();
if (!renderer->loadMesh("Models/projectile.fbx"))
{
	// Enter the application's resource error path.
	return;
}
renderer->setTexture(0,
	CTextureManager::getInstance()->getTexture("Textures/projectile.png"));
renderer->setMaterialType(Particle::CMeshParticleRenderer::Soild);
renderer->setVelocityDirection(true);
renderer->SizeX = renderer->SizeY = renderer->SizeZ = 1.0f;
group->setRenderer(renderer);
```

`Soild` is the current API spelling. `setVelocityDirection(true)` aligns the mesh Z axis with particle velocity; author the model accordingly and avoid a zero direction when using velocity alignment. A mesh particle is not a separate `CRenderMesh` with a skeleton or arbitrary per-object material collection. Use compatible geometry/materials for batching, and validate the loaded mesh, shader, textures, scale, and supported backend. The particle factory/component manages the created particle resources; do not separately delete a borrowed group/renderer during use.

### Choose where application state lives

| Storage | Example state | Removal bookkeeping |
| --- | --- | --- |
| Parallel array indexed by `p.Index` | Damage, team, piercing count, stable logical ID | Initialize on birth; swap entries in `OnSwapParticleData`; retire the dead slot. |
| `p.UserData` | Flight state machine, copied destination, pooled object record | Pointer follows particle content swaps; free/recycle it on death and explicit teardown. |

A slot index is not a stable logical ID. Assign an independent ID when hit deduplication, events, or replay need identity across swaps. Store copied target positions or validated handles rather than unmanaged target-data pointers.

For a parallel array, `OnParticleBorn()` resizes and fully resets the slot; spawning code writes the requested damage/team after `addParticleVelocity()` returns. `OnSwapParticleData(a, b)` swaps the two application records before the engine swaps particle contents. `OnParticleDead(p)` invalidates that slot. A simple `pop_back()` is valid only for optimized tail removal; bulk clear reports particles in ascending order, so use slot invalidation as in section 5 or a deliberately separate bulk-clear path.

### Spawn a mesh particle with UserData

The remaining fragments belong to an application `CParticleObjectController : public Particle::IParticleCallback`, bound to a dedicated group through `addCallback(this)`. It stores `Particle::CGroup* m_group`, initialized to null. The callback object and group must outlive active use of their relationship.

```cpp
struct SFlightData
{
	enum EState { Rising, Falling };
	EState State = Rising;
	core::vector3df Target;
};

int CParticleObjectController::spawn(const core::vector3df& position,
	const core::vector3df& velocity, const core::vector3df& target)
{
	if (!m_group)
		return -1;
	int id = m_group->addParticleVelocity(0, position, velocity);
	if (id < 0)
		return -1;

	auto* data = new SFlightData();
	data->Target = target;
	Particle::CParticle& particle = m_group->getParticles()[id];
	particle.Immortal = true;
	particle.UserData = data;
	return id;
}
```

The host/group transform is identity and positions are world-space in this example. Bind the callback while the group is empty, with positive emitter lifetime and automatic emission disabled. The controller owns every `SFlightData` in this dedicated group. A high-volume application can allocate these records from a pool instead of using one `new` per spawn; preserve the same ownership rules.

### Update motion and collision through callbacks

`CGroup::update()` performs birth/emission, built-in motion/lifetime and custom particle systems, then `OnParticleUpdate()`, then dead-particle compaction and bounds. Consequently `LastPosition` and `Position` describe the built-in movement segment when this callback runs. The callback's `dt` is milliseconds; convert to seconds for speed/acceleration calculations.

For normal ballistic projectiles, let the particle system integrate velocity and gravity, then perform collision tests against the segment `LastPosition -> Position`. Query a spatial structure for candidates and use a swept/segment test rather than only testing the final point. Handle a zero-length segment, pick the earliest valid hit along travel, apply the application effect once, and set `Life = -1.0f` when consumed. For piercing/bounce, update the associated state/velocity and define repeated-hit policy explicitly.

For custom flight, replace the built-in result using `LastPosition`; do not add a second displacement to the already integrated `Position`:

```cpp
void CParticleObjectController::OnParticleUpdate(
	Particle::CParticle* particles, int count, Particle::CGroup* group, float dt)
{
	const float seconds = dt * 0.001f;
	for (int i = 0; i < count; ++i)
	{
		Particle::CParticle& p = particles[i];
		if (p.Life < 0.0f || !p.UserData)
			continue;
		auto* data = static_cast<SFlightData*>(p.UserData);
		if (data->State == SFlightData::Rising)
		{
			const float speed = core::min_(p.Velocity.Y + 14.0f * seconds, 24.0f);
			p.Velocity.set(0.0f, speed, 0.0f);
			p.Position = p.LastPosition + p.Velocity * seconds;
			if (p.Position.Y >= 30.0f)
			{
				data->State = SFlightData::Falling;
				p.Position.X = data->Target.X;
				p.Position.Z = data->Target.Z;
				p.Velocity.set(0.0f, -48.0f, 0.0f);
			}
		}
		else
		{
			p.Velocity.set(0.0f, -48.0f, 0.0f);
			p.Position = p.LastPosition + p.Velocity * seconds;
			if (p.Position.Y <= data->Target.Y)
			{
				p.Position = data->Target;
				p.Life = -1.0f;
				// Copy an impact event/position to an application queue here.
			}
		}
	}
}
```

This illustrates a deliberate rise/reposition/fall trajectory; the height and speeds are example parameters. Configure zero gravity/friction and no competing position systems for this custom path. Keep `LastPosition` as the segment start for moving objects, unlike anchored icons which synchronize both positions to one point. A deliberate teleport/reposition must have its own collision policy rather than treating that jump as normal travel.

Callbacks are part of particle simulation, not per-camera render preparation. They also run when `group->Visible` is false, but that path only updates lifetime before callbacks, not built-in motion. If invisible projectiles must keep moving, keep simulation visibility enabled or provide custom movement that does not rely on stale `LastPosition`; do not equate render hiding with pausing simulation.

### Defer spawning and structural changes

Never add particles to the group being traversed inside `OnParticleUpdate()` (including indirect calls from hit/damage callbacks). `addParticleVelocity()` can reallocate its active array; both your `particles` pointer and the engine's pointer used after the callback would become invalid. Do not clear/reload/destroy the group or remove callbacks while it is iterating callbacks either.

Queue copied spawn commands containing position, velocity, and application parameters. Drain them from a known safe component/system phase before the next particle-group update, outside the current `CGroup::update()` call. A death callback that requests secondary projectiles must queue them too. Separate impact groups still need their own lifecycle/emission scheduling; copied impact events are a straightforward way to avoid reentrant changes. The queue introduces an explicit simulation-step delay, so account for that in timing-sensitive behavior.

### Free UserData on death and on teardown

The engine swaps `UserData` together with particle contents, so do not swap it again in `OnSwapParticleData()`. Only swap external parallel arrays there. `CParticle` does not own or delete `UserData`:

```cpp
void CParticleObjectController::OnParticleDead(Particle::CParticle& particle)
{
	delete static_cast<SFlightData*>(particle.UserData);
	particle.UserData = nullptr;
}
```

Use the matching pool-release operation instead of `delete` for pooled records. Assign `Life = -1.0f` for explicit removal; immortal particles still obey this request. Emit gameplay impact events in the collision/state transition path, not unconditionally in `OnParticleDead()`: particles also die from timeout, cancellation, Stop, or bulk clearing.

Group destruction does not individually report all particle deaths. Implement explicit shutdown while the group is alive: stop spawn production, clear pending commands, call `group->clearParticles()` with this controller still registered so death callbacks release its records, then remove the callback and null the group pointer. This assumes a dedicated group and a safe phase outside callback iteration. Do not unregister first and leave allocated UserData behind.

As a fallback for group-first destruction, release remaining records in `OnGroupDestroy()` while the group particle storage is still valid, then invalidate the borrowed pointer:

```cpp
void CParticleObjectController::OnGroupDestroy()
{
	if (m_group)
	{
		auto& particles = m_group->getParticles();
		for (u32 i = 0; i < particles.size(); ++i)
		{
			delete static_cast<SFlightData*>(particles[i].UserData);
			particles[i].UserData = nullptr;
		}
	}
	m_group = nullptr;
	// Clear the controller's copied pending spawn/event commands as appropriate.
}
```

This read/cleanup path relies on the current destructor invoking `OnGroupDestroy()` before destroying particle storage. Do not spawn, clear particles, or change the callback list from it. The earlier icon recipe only needs to invalidate owners; a UserData owner additionally needs this memory cleanup. Keep the controller alive until it has unregistered or received group destruction, and give exactly one participant ownership of each record to avoid double deletion.

Validate middle-particle removal, timeout, same-step multiple hits, secondary-spawn queues, group clear/reload, controller-first teardown, group-first teardown, and both quad/mesh renderers. Profile allocation, collision queries, CPU update time, and draw submissions with realistic object counts before selecting this representation for an application.

## Engine source references

- [CGroup.h](../Projects/Skylicht/Components/ParticleSystem/Particles/CGroup.h) and [implementation](../Projects/Skylicht/Components/ParticleSystem/Particles/CGroup.cpp): manual spawn, callbacks, optimized swap-removal, bulk clear, group destruction.
- [CParticleGroupSystem.cpp](../Projects/Skylicht/Components/ParticleSystem/CParticleGroupSystem.cpp): group updates, host transforms, bounds.
- [CParticleComponent.cpp](../Projects/Skylicht/Components/ParticleSystem/CParticleComponent.cpp): group loading, registered systems, Stop/clear behavior.
- [CWorldTransformData.h](../Projects/Skylicht/Engine/Transform/CWorldTransformData.h): local/world position access.
- [CEntityManager.cpp](../Projects/Skylicht/Engine/Entity/CEntityManager.cpp): query/update order, removal callbacks, teardown.

- [CMeshParticleRenderer.h](../Projects/Skylicht/Components/ParticleSystem/Particles/Renderers/CMeshParticleRenderer.h): mesh loading, blend mode, velocity alignment.
- [CParticle.cpp](../Projects/Skylicht/Components/ParticleSystem/Particles/CParticle.cpp) and [CParticleSystem.cpp](../Projects/Skylicht/Components/ParticleSystem/Particles/Systems/CParticleSystem.cpp): UserData swaps/ownership, motion integration, time units.
