# First initialization: render early, load in stages

This tutorial describes a staged Skylicht startup: render a small loading UI first, initialize the remaining application behind it, and load a world only when needed. It complements [Setup.md](Setup.md) and [Architecture.md](Architecture.md).

Examples use standard `.zip` archives and illustrative application asset paths. Supply your own GUI, sprites, fonts, and world assets. Package labels and loading states are application choices; the engine APIs described here were checked against this checkout.

## 1. What the first visible frame needs

The device and engine runtime must already exist, as described in Setup.md. The first visible UI then needs only:

1. Access to BuiltIn assets, through loose files or a mounted archive.
2. The shaders used by that UI.
3. A small scene, zone, UI camera, and canvas or other visible geometry.
4. The font, atlas, and textures actually referenced by that screen.
5. An update/render loop that returns control every frame.

A camera alone produces no loading indicator. Mounting an archive makes files accessible; it does not preload every texture, compile every shader, or instantiate a scene. Importing a GUI can load its referenced sprites, textures, and fonts, so keep those dependencies small too.

| Phase | Load now | Defer |
| --- | --- | --- |
| Bootstrap | BuiltIn, selected UI shaders, minimal UI/font | Broad shader families, application config, SDKs, world assets |
| Splash | Splash GUI and its dependencies | Full menu and gameplay content |
| Application initialization | Shared UI, required shaders, settings/config/localization, services, sound setup | Level-specific objects and effects |
| Enter a world | Selected scene, render pipeline, navigation, actors and effects | Unused levels and unrelated content |

## 2. Mount BuiltIn.zip and load a few shaders

For a bundled application, mount the standard ZIP archive through the file system:

```cpp
io::IFileSystem* fs = getApplication()->getFileSystem();
if (!fs->addFileArchive(
	getApplication()->getBuiltInPath("BuiltIn.zip"), false, false))
{
	// Report the failure and stop dependent initialization.
	return;
}
```

This assumes the application has been packaged with BuiltIn.zip at its built-in asset location and ZIP support is enabled in its Irrlicht build. Standard ZIP archives do not require registration of a project-specific archive loader. In loose-file development, make the BuiltIn directory accessible instead of mounting an archive. Preserve paths inside archives, such as `BuiltIn/Shader/Basic/VertexColor.xml`.

Load only the shaders needed by the initial UI. For example:

```cpp
CShaderManager* shaders = CShaderManager::getInstance();
shaders->loadShader("BuiltIn/Shader/Basic/VertexColor.xml");
shaders->loadShader("BuiltIn/Shader/Basic/VertexColorAlpha.xml");
shaders->loadShader("BuiltIn/Shader/Basic/TextureColorAlpha.xml");
shaders->loadShader("BuiltIn/Shader/Basic/TextureColorAlphaDesaturate50.xml");
```

This is an example selection, not a universal minimum. Choose according to the loading UI's materials. Defer `initBasicShader()`, `initMobileSGShader()`, and other broad initialization until the UI can render; only initialize shader families your application uses.

Create a small scene/zone, an `OrthoUI` camera, and visible UI. Given a bootstrap zone:

```cpp
CGameObject* cameraObject = zone->createEmptyObject();
CCamera* camera = cameraObject->addComponent<CCamera>();
camera->setProjectionType(CCamera::OrthoUI);
```

Use a small canvas/logo or text with an explicitly loaded font. Ensure its materials and textures are available before drawing it. Keep the first indicator independent of large menu atlases or world packages.

In the frame loop, update the owned scene and UI as appropriate, then render the UI camera:

```cpp
CGraphics2D::getInstance()->render(camera);
```

Return to the main loop before expensive work. Let the first loading frame complete before advancing into heavier initialization. Waiting an arbitrary number of update frames is not an engine requirement and does not establish a fixed time delay.

## 3. Make splash assets accessible

If the bootstrap indicator is deliberately tiny, a second stage can mount or download a small Splash.zip containing a richer loading GUI and its dependencies. Keep menu and level archives for later stages.

For a browser application, download required archives asynchronously, poll completion, update the visible progress indicator, and mount each completed ZIP. Provide retry/error handling. For native applications, mount installed ZIPs through `addFileArchive(...)` and check the result.

Downloading a large archive delays access to its contents even if none of its textures have been instantiated. Organize archives around when content becomes necessary, and avoid downloading all world content before showing the splash.

A splash scene typically contains a canvas, imported GUI, and OrthoUI camera. After GUI import, apply scaling and update canvas entities as required by your UI setup. Animate an icon or update progress while initialization proceeds. Give this scene an explicit owner responsible for its eventual destruction.

## 4. Initialize the rest through a state machine

An application can use this order:

```text
ShowSplash -> LoadSharedUI -> InitShaders
  -> LoadSettingsAndConfig -> InitAudio -> InitServices -> Ready
```

Adjust dependencies for your application. External services, authentication, analytics, purchases, and consent flows are optional application concerns; Skylicht UI rendering does not require them.

| State | Work |
| --- | --- |
| ShowSplash | Allow a loading-screen frame to render before heavy work |
| LoadSharedUI | Load shared sprite/font resources under Global |
| InitShaders | Initialize required shader families in separate substates |
| LoadSettingsAndConfig | Load saved settings, configuration and localization before dependent systems |
| InitAudio | Prepare audio file access, apply saved volumes and initialize required sounds |
| InitServices | Start optional services and poll asynchronous completion |
| Ready | Enter the menu or begin loading the selected world |

Use one bounded unit of work per update, then return so the screen can render. Keep scene/UI animation and progress updates active while waiting for futures. Do not call a blocking future `.get()` on the frame thread before completion is known.

A state machine alone does not make loading asynchronous or eliminate stalls. Loading many config files in one state or compiling an entire shader family can still freeze the indicator. Profile those steps and split large lists into smaller batches. A state named ShowSplash must actually wait for a completed render; changing a state name does not present a frame.

Keep graphics resource creation and driver calls on the render/main thread unless the engine explicitly supports another path. Background work is suitable for independent I/O or parsing with safe ownership and synchronization.

### Audio needs its own file-access lifetime

Initialize sound after settings/config so saved volume and sound definitions are available. Audio opens files and reads file information and audio data on its own thread, independently of the main/render thread.

**Keep audio assets in a separate bundle, such as Audio.zip, instead of mixing them into bundles read by scene, texture, or UI loaders.** Concurrent seek/read operations on a shared underlying file handle can race and corrupt reads. Give the audio bundle an independent file handle and keep it mounted while audio readers are active. Separating bundle names alone is insufficient if their archive readers still share the same underlying handle.

On Android, archive entries inside the same APK can share an underlying file handle. Placing audio in Audio.zip inside that APK alone may not isolate concurrent seek/read operations. A suitable packaging strategy is to copy Audio.zip to application storage, close the copy writer, mount that external ZIP, and only then start audio readers. Ensure resource lookup selects the external archive. Use application-defined version or integrity metadata to decide when an installed copy needs replacement.

On platforms where the installed archive can be opened independently, mount Audio.zip directly. Keep packaging paths and runtime lookup consistent. Handle archive preparation failure before starting readers, with an explicit retry or sound-disabled path. Stop audio readers before replacing or unmounting archives they use.

## 5. Resource package names: setCurrentPackage

Use the texture manager's current package to group resources by lifetime:

```cpp
CTextureManager::getInstance()->setCurrentPackage("Menu");
```

The API for this operation is `setCurrentPackage(...)`, rather than `SetPackageName(...)`. Materials also expose `setPackage(...)`, but the normal manager loading workflow takes the package from the texture manager's current package.

A resource package is a lifetime label, independent of archive filenames and folders. Mounting Menu.zip does not automatically tag its resources as Menu.

| Example label | Intended lifetime |
| --- | --- |
| Global | Shared UI/resources that survive transitions |
| Menu | Menu resources released when entering a world |
| Gameplay | Loading/world resources released when leaving gameplay |

These labels are illustrative; choose names matching your application. Global is also the texture manager's default package name.

Set the label **before** loading resources and their dependencies:

```cpp
CTextureManager* textures = CTextureManager::getInstance();
std::string previousPackage = textures->getCurrentPackage();
textures->setCurrentPackage("Global");
CSpriteManager::getInstance()->loadSprite("UI/Common.spritedata");
textures->setCurrentPackage(previousPackage.c_str());
```

`UI/Common.spritedata` is an illustrative application asset. A helper doing a temporary load should restore the previous package, including on failure paths. A view may intentionally leave a new phase package active.

The engine records labels at load/registration time:

- Texture registration stores the current package and skips already registered texture pointers.
- Sprite loading stores the current package but returns cached sprites without relabeling them.
- Material manager loading/creation assigns the current package to new materials.

Changing the current package does not move cached resources or create independent copies. Load truly shared resources under Global first. Loading a Menu resource again under Gameplay does not make it safe to release Menu while gameplay still references it. The current package is mutable singleton state; interleaved loaders must coordinate it.

## 6. Release outgoing data at a clear transition boundary

Use a loading scene whose resources survive cleanup of the outgoing package. It can use shared resources under Global or dedicated resources under the incoming phase's package. Stop outgoing input, callbacks and simulation, and detach references to the outgoing scene before removing its assets.

Give each scene one clear owner. Destroy an independently owned scene explicitly, or let its documented framework owner destroy it. Do not assume that assigning a scene pointer to NULL, changing the active camera, or installing another scene deletes the previous scene. A scene update may process queued object removals; it does not replace explicit ownership management.

After outgoing scene/UI owners have been destroyed and pending rendering has been synchronized where necessary, a cleanup sequence can be:

```cpp
// All outgoing users have stopped; the loading UI uses another package.
getVideoDriver()->finish();

// Only if no other active scene/import needs these global mesh caches:
CMeshManager::getInstance()->releaseAllPrefabs();
CMeshManager::getInstance()->releaseAllInstancingMesh();

// Clear application caches that retain outgoing assets before this point.
CSpriteManager::getInstance()->releaseSprite("Menu");
CMaterialManager::getInstance()->releaseAllMaterials("Menu");
CTextureManager::getInstance()->removeTexture("Menu");
```

`finish()` synchronizes GPU work; it does not destroy CPU-side owners. Use it at a deliberate transition boundary rather than every frame. Start loading incoming resources after cleanup so broad cache release cannot remove newly imported entries.

Understand the scope of each release operation:

- Mesh cache release calls above are global, not filtered by Menu. Omit them when another scene still needs those caches.
- `releaseSprite("Menu")` deletes matching sprite objects. Their frame pointers must no longer be used. Sprite destruction also removes its owned image textures through the texture manager; avoid sharing those images across independently released sprite lifetimes.
- `releaseAllMaterials("Menu")` drops manager-held references. Remaining owners can keep materials alive and produce leak diagnostics.
- `removeTexture("Menu")` removes matching textures only when their reference count is 1; referenced textures are skipped and logged. Release their owners and retry package cleanup if necessary.

Package release is not a universal garbage collector. Archive mounts, shader caches, audio emitters, physics, animation caches, application data, and render targets have separate lifecycles. Destroy or reset them through their own owners when appropriate. Retain Global assets while any active UI or scene needs them.

## 7. Load only the selected world's requirements

Set the incoming package before imports and dependent resource loads:

```cpp
CTextureManager::getInstance()->setCurrentPackage("Gameplay");
```

A world-loading state machine can then advance through:

```text
LoadScene -> InitRenderPipeline -> InitMaterials
  -> InitNavigation -> InitEffects -> InitActors
  -> InitCamerasAndSystems -> Ready
```

For staged scene import, call `CSceneImporter::beginImportScene(...)` with the target scene and asset path, then advance it through successive `CSceneImporter::updateLoadScene()` calls. Use `CSceneImporter::getLoadingPercent()` for that phase's progress.

Initialize one effect pool entry or actor type per update when those operations are expensive. Build the required list from the selected level's data rather than initializing every type in the application. Keep the dedicated loading UI visible throughout.

Finish required materials, navigation, render pipeline, actors, cameras, and systems before handing control to the world. Switch scene references and active/UI cameras together so rendering cannot use a camera from a deleted loading scene. Delete the loading scene through its owner after the handoff. Treat import progress as one phase's progress: reaching 100% there does not mean all later initialization is ready.

## 8. Verify startup and repeated transitions

Measure time to the first visible frame, the longest loading frame, and memory before/after repeated menu/world transitions. Test a cold start as well as cached startup. Check that:

- The first loading UI renders before broad shader, config, and service initialization.
- Each loading dependency is accessible before use, and failures have a retry or fallback path.
- Shared resources retain Global ownership; temporary loads restore the intended package.
- The loading screen survives outgoing package cleanup.
- No view holds sprite frames, materials, textures, or cameras belonging to a released scene/package.
- Audio can read safely while later assets load.
- Repeated transitions do not accumulate skipped textures, material references, scenes, or application caches.

Engine implementation references: [texture packages](../Projects/Skylicht/Engine/TextureManager/CTextureManager.cpp), [material packages](../Projects/Skylicht/Engine/Material/CMaterialManager.cpp), [sprite packages](../Projects/Skylicht/Engine/Graphics2D/SpriteFrame/CSpriteManager.cpp), and [sprite destruction](../Projects/Skylicht/Engine/Graphics2D/SpriteFrame/CSpriteFrame.cpp).
