# Cameras: menu composition, culling, and gameplay transitions

This guide follows [Setup.md](Setup.md), [Architecture.md](Architecture.md), and [FirstInit.md](FirstInit.md). It describes engine-level patterns for a menu with 2D controls and a 3D preview, plus smooth gameplay camera transitions. All examples use generic application objects and engine APIs; no private project source, assets, identifiers, or framework is required.

Fragments assume the normal Skylicht runtime is initialized, the needed shaders/materials are loaded, and `scene` and `zone` are valid application-owned objects. Use the `Skylicht` namespace and the Irrlicht types available through the application's existing headers. Include the relevant engine headers, such as `Camera/CCamera.h`, `Camera/CCameraBrain.h`, `Graphics2D/CCanvas.h`, `Graphics2D/GUI/CGUIImage.h`, `RenderPipeline/CForwardRP.h`, `Tween/CTweenFloat.h`, and `Tween/CTweenManager.h`.

## 1. Choose a camera for each job

| Job | Projection | Render entry point |
| --- | --- | --- |
| Menu controls, text, preview image | `CCamera::OrthoUI` | `CGraphics2D::render(uiCamera)` |
| Perspective menu background or model preview | `CCamera::Perspective` | Render pipeline with a screen or texture target |
| Orthographic world/model view | `CCamera::Ortho` | Render pipeline |
| Smooth gameplay viewpoint changes | Output `CCamera` with `CCameraBrain` | Render pipeline using the output camera |

Creating a camera does not schedule a render pass or make it globally active. The application explicitly supplies a camera to each render call. A brain selects and blends a viewpoint for one output camera; simultaneous views still require separate render calls.

Keep cameras unparented, or under identity transforms, for these examples. The current camera view and brain use local transform position/orientation, so do not assume arbitrary parent transforms work like world-space camera rigs.

## 2. Object layers and camera masks

Both values are `u32` bitmasks. A layer value is a bit, such as `1u << 2`, rather than the layer index `2`. An object may belong to more than one layer. The layer test accepts an object when:

```cpp
(objectLayer & cameraMask) != 0
```

Define application-specific bits consistently. These example assignments are not reserved engine constants:

```cpp
const u32 WorldLayer = 1u << 0;
const u32 PreviewLayer = 1u << 1;
const u32 UILayer = 1u << 2;

previewObject->setCullingLayer(PreviewLayer);
worldCamera->setCullingMask(WorldLayer);
previewCamera->setCullingMask(PreviewLayer);
canvas->getGameObject()->setCullingLayer(UILayer);
uiCamera->setCullingMask(UILayer);
```

The current defaults for a GameObject's visible data and a camera mask are both `1` (bit 0), not all bits. A newly assigned preview layer will disappear from a default camera until that camera includes it.

| Camera mask | Objects passing the layer test |
| --- | --- |
| `WorldLayer` | World objects |
| `PreviewLayer` | Preview objects |
| `WorldLayer \| PreviewLayer` | Both categories |
| `0` | None |
| `~0u` | Any nonzero layer bit |

Use `camera->setCullingMask(camera->getCullingMask() & ~PreviewLayer)` to exclude previews from an existing mask. `object->setCullingLayerOnOff(PreviewLayer, true)` adds that bit without replacing other memberships; passing `false` removes it. `setVisible(false)` controls general visibility, while a mask selects visibility per camera. Passing the layer test does not bypass frustum culling, hidden parents, or other renderer-specific conditions, and it does not stop simulation or input.

For a hierarchy, assigning only the container's layer does not recursively relabel its children. Given a `CContainerObject* previewRoot`, assign both explicitly:

```cpp
previewRoot->setCullingLayer(PreviewLayer);
previewRoot->setCullingLayerForChild(PreviewLayer);
```

Apply the policy again to children added later. `setCullingOnOffForChild(bit, on)` changes child memberships without replacing the other bits. Components that create additional render entities must propagate layers through their culling-layer handling; inspect the component if a generated instance is visible in the wrong pass.

For GUI, `CGraphics2D` filters the GameObject containing each canvas using the UI camera mask. Separate canvases when groups of GUI elements need different camera masks; assigning a layer to a 3D preview object does not assign a layer to its GUI image.

## 3. Create the menu's 2D camera

```cpp
CGameObject* uiCameraObject = zone->createEmptyObject();
CCamera* uiCamera = uiCameraObject->addComponent<CCamera>();
uiCamera->setProjectionType(CCamera::OrthoUI);
uiCamera->setCullingMask(UILayer);

CGameObject* canvasObject = zone->createEmptyObject();
CCanvas* canvas = canvasObject->addComponent<CCanvas>();
canvasObject->setCullingLayer(UILayer);
```

`OrthoUI` uses an identity view matrix: the origin is at the top left, X grows right, and Y grows down. Moving this camera is not how to pan a screen-space menu. Position GUI elements or their parent instead.

By default the projection follows the current render target dimensions. For a fixed design coordinate system, explicitly opt into a custom size:

```cpp
uiCamera->setOrthoUISize(1280.0f, 720.0f);
uiCamera->enableCustomOrthoUISize(true);
uiCamera->recalculateProjectionMatrix();
```

Choose canvas scaling, aspect preservation, and input coordinate mapping to match that design size. A custom projection alone does not implement responsive layout. These size setters do not mark projection changes in the current implementation, so recalculate after changing them.

Update the scene once per frame. In the render phase, bind the intended screen target, finalize the camera for that target, then draw:

```cpp
getVideoDriver()->setRenderTarget(NULL, false, false);
uiCamera->endUpdate();
CGraphics2D::getInstance()->render(uiCamera);
```

The application owns frame begin/end and screen clearing. Avoid clearing after an earlier background pass that should remain visible.

## 4. Create a 3D menu or preview camera

```cpp
CGameObject* previewCameraObject = zone->createEmptyObject();
CCamera* previewCamera = previewCameraObject->addComponent<CCamera>();
previewCamera->setProjectionType(CCamera::Perspective);
previewCamera->setCullingMask(PreviewLayer);
previewCamera->setFOV(40.0f); // Degrees.
previewCamera->setNearValue(0.1f);
previewCamera->setFarValue(100.0f);
previewCamera->lookAt(
	core::vector3df(0.0f, 1.5f, -4.0f),
	core::vector3df(0.0f, 1.0f, 0.0f),
	Transform::Oy);
```

Choose distances and clip planes for your model's actual bounds. Include its lights, environment, and materials as required by the selected pipeline. A camera does not provide lighting or load a model.

For an orthographic 3D preview, use `CCamera::Ortho` and `setOrthoScale(...)`, then recalculate projection. In the current implementation its visible half-height is `tan(FOV / 2) * orthoScale`; the scale is not directly the height in world units. Use `OrthoUI` for screen-space menus.

A full-screen 3D menu background can render directly to `NULL` through the pipeline, followed by the UI pass. For a preview embedded among GUI controls, render into a texture first.

## 5. Render 3D to a texture and display it in GUI

The render order is:

```mermaid
flowchart LR
    Scene[Updated 3D scene] --> Preview[Preview camera and layer mask]
    Preview --> Pipeline[3D render pipeline]
    Pipeline --> RTT[Render target texture]
    RTT --> Image[CGUIImage inside menu canvas]
    Image --> UI[OrthoUI camera renders GUI to screen]
```

### Allocate resources once

This minimal example uses a dedicated forward pipeline without linear-RGB conversion or emission. Select compatible materials/shaders, and initialize the shader families required by your pipeline. For transparent previews displayed on a GUI image, do not attach a post-processor or use bloom, FXAA, or other post effects: those passes can destroy transparent alpha. Keep post effects confined to separate world/screen rendering paths.

```cpp
core::dimension2du previewSize(512, 512);
ITexture* previewTexture = getVideoDriver()->addRenderTargetTexture(
	previewSize, "MenuPreviewRTT", video::ECF_A8R8G8B8);
if (!previewTexture)
{
	// Report failure or show a static fallback image.
	return;
}

CForwardRP* previewPipeline = new CForwardRP(false, false);
previewPipeline->initRender(previewSize.Width, previewSize.Height);
previewCamera->setAspect(
	(float)previewSize.Width / (float)previewSize.Height);

CGUIImage* previewImage = canvas->createImage(
	core::rectf(100.0f, 100.0f, 500.0f, 500.0f));
previewImage->setImage(previewTexture);
```

Keep these pointers as members of the menu/resource owner. Give concurrently allocated targets distinct names. `CGUIImage::setImage` stores a pointer and sets its source rectangle; it does not take ownership of the texture. Set the GUI image's rectangle/layout to the intended display size, preserving the target aspect ratio to avoid stretching.

### Render every required frame

After the normal scene update, render the preview before the GUI samples it:

```cpp
IVideoDriver* driver = getVideoDriver();
core::recti savedViewport = driver->getViewPort();

// This path starts from the screen target.
// Clear BOTH color and depth so old preview frames do not remain.
driver->setRenderTarget(previewTexture, true, true,
	video::SColor(0, 0, 0, 0));
previewCamera->endUpdate();
previewPipeline->render(previewTexture, previewCamera,
	scene->getEntityManager(), core::recti());

// Preserve screen content drawn before the preview pass.
driver->setRenderTarget(NULL, false, false);
driver->setViewPort(savedViewport);
uiCamera->endUpdate();
CGraphics2D::getInstance()->render(uiCamera);
```

`CCamera::endUpdate()` reads the current render target size. Bind the target before finalizing its camera; set an explicit aspect for non-screen targets and update it on resize. A sub-viewport also needs its own width/height aspect rather than an assumption that the full target has the same shape.

The direct `CForwardRP(false, false)` path does not automatically clear a 2D target in `render()`, which is why the explicit clear above matters. Other pipelines may clear intermediate targets themselves. Check the entire pipeline chain before assuming it preserves the transparent background. Skyboxes, final conversion shaders, FXAA, bloom, and material alpha writes can change the output alpha. Verify transparent composition on each supported graphics backend; an RGBA format alone does not guarantee it.

For a shared pipeline, save and restore any modified camera mask, post-processing options, viewport, and target on every exit path. The example uses a dedicated preview camera/pipeline to keep its configuration explicit. If called from a nested offscreen pass, restore that caller's target instead of always binding `NULL`.

Prefer a GUI image for ordinary panel composition so GUI ordering and transforms remain in the normal GUI path. A custom `CGUIElement::OnRender` callback can insert special composition at a specific GUI position, but flush pending `CGraphics2D` batches before direct driver drawing and restore any render state it changes. Never render into a texture while the same pass samples it.

### Transparent backgrounds: preserve alpha through every pass

For a model floating over a menu background, all of these conditions must hold:

1. Allocate an alpha-capable target, for example `ECF_A8R8G8B8`.
2. Clear the scene color target to `SColor(0, 0, 0, 0)` and clear depth each new preview frame. If the forward pipeline renders through an intermediate target, that intermediate clear must also have alpha zero. `CBaseRP::setClearColor(...)` controls the forward pipeline's clear color globally; coordinate it with other passes instead of changing it silently inside a GUI callback.
3. Use the preview camera mask to exclude opaque backgrounds, floors, or skyboxes that would cover the transparent pixels.
4. Render this transparent preview without bloom or post effects. Do not attach a post-processor to its dedicated pipeline.
5. Composite using an alpha-blended shader, such as `TextureColorAlpha`, over GUI content that has already been drawn.

The current engine shaders explain why disabling selected effects is necessary:

| Pass | Current alpha behavior | Transparent preview policy |
| --- | --- | --- |
| `TextureLinearRGB` | Converts RGB and retains sampled alpha multiplied by vertex alpha; uses a solid base material | Suitable as an intermediate copy/conversion, not the final alpha-blended GUI composition |
| `PostEffectManualExposure` | Retains the input color alpha | Retains alpha in this shader, but leave post-processing disconnected for the transparent preview policy |
| Bloom (`BloomFS.glsl`) | Writes `vec4(m, 1.0)` | Disable for this transparent preview path |
| FXAA (`FXAAFS.glsl`) | Contains an output path using luminance as alpha | Disable for this preview path; do not treat its output alpha as coverage |
| `TextureColorAlpha` | Samples texture times vertex color, with `TRANSPARENT_ALPHA_CHANNEL` blending | Use for the final preview-over-GUI draw |

**Transparent image preview policy: do not use bloom or post effects.** Use the dedicated `CForwardRP(false, false)` example without attaching a `CPostProcessorRP`. If reusing a forward pipeline, `setPostProcessor(NULL)` disconnects its post-processor, but linear-RGB conversion, emission, and chained passes still need review. Prefer a dedicated pipeline to avoid changing shared world rendering state.

The shader table explains the failure mechanism; it is not a recommendation to keep selected post effects enabled. Although some individual conversion/exposure shaders retain alpha, a complete post-effect chain can overwrite it. Temporarily disabling only FXAA and bloom in an existing application is not equivalent to disconnecting all post-processing. Keep the transparent image path free of post effects and restore any shared pipeline configuration changed for a temporary pass.

### Insert the preview at a particular GUI draw position

An alternative to `CGUIImage` is an `OnRender` callback on a plain GUI element. Render the model to its texture before rendering GUI; the callback only composites that existing texture. This is useful when the menu background should draw first, the preview next, and foreground controls afterward.

The following generic callback uses an explicit destination rectangle. The application owns `previewTexture` and `previewPipeline`, and must keep them alive until the callback is detached. The alpha shader must already be loaded.

```cpp
int compositeShader = CShaderManager::getInstance()->getShaderIDByName(
	"TextureColorAlpha");
previewSlot->OnRender = [previewTexture, previewPipeline, compositeShader](CGUIElement*)
{
	CGraphics2D::getInstance()->flush();
	IVideoDriver* driver = getVideoDriver();
	core::matrix4 world = driver->getTransform(video::ETS_WORLD);
	core::matrix4 view = driver->getTransform(video::ETS_VIEW);
	core::matrix4 projection = driver->getTransform(video::ETS_PROJECTION);

	core::dimension2du screen = driver->getScreenSize();
	previewPipeline->beginRender2D((float)screen.Width, (float)screen.Height);
	SMaterial material;
	material.setTexture(0, previewTexture);
	material.MaterialType = compositeShader;
	core::dimension2du source = previewTexture->getSize();
	previewPipeline->renderBufferToTarget(
		100.0f, 100.0f, 400.0f, 400.0f,
		0.0f, 0.0f, (float)source.Width, (float)source.Height,
		material, true, false);

	driver->setTransform(video::ETS_WORLD, world);
	driver->setTransform(video::ETS_VIEW, view);
	driver->setTransform(video::ETS_PROJECTION, projection);
};
```

This fragment assumes a full-screen viewport and screen-pixel coordinates. It deliberately creates its own 2D projection and restores the previous matrices. Adapt the destination rectangle to layout, scaling, viewport, and clipping requirements. The callback does not automatically use the element's transform or rectangle. The shorter `renderBufferToTarget(sx, sy, sw, sh, material)` overload takes a source rectangle and fills the pipeline's stored 2D viewport dimensions; its four numbers are not a destination rectangle.

`renderBufferToTarget` disables depth testing and depth writing for its quad. Its default `flipY=true` flips texture coordinates on non-Direct3D11 drivers, handling the engine's render-target orientation convention. A plain `CGUIImage` uses the ordinary image batch path, so do not assume it performs the same render-target flip. Check orientation with an asymmetric test model or marker, and adapt UVs or choose this explicit composition path when needed.

Call `flush()` before direct drawing so queued GUI backgrounds appear before the preview, rather than being submitted over it later. Restore changed state so subsequent GUI elements render correctly. Detach `previewSlot->OnRender` before freeing its captured resources. Avoid drawing an opaque element over the preview afterward; alpha reveals only content already present underneath.

Shader references: [TextureColorAlpha.xml](../Assets/BuiltIn/Shader/Basic/TextureColorAlpha.xml), [TextureColorFS.glsl](../Assets/BuiltIn/Shader/Basic/GLSL/TextureColorFS.glsl), [TextureLinearRGBFS.glsl](../Assets/BuiltIn/Shader/Basic/GLSL/TextureLinearRGBFS.glsl), [PostEffectManualExposureFS.glsl](../Assets/BuiltIn/Shader/PostProcessing/GLSL/PostEffectManualExposureFS.glsl), [BloomFS.glsl](../Assets/BuiltIn/Shader/PostProcessing/GLSL/BloomFS.glsl), and [FXAAFS.glsl](../Assets/BuiltIn/Shader/PostProcessing/GLSL/FXAAFS.glsl).

### Resize and release

When the target size changes, detach the image with `previewImage->setImage(NULL)`, remove the old texture through `driver->removeTexture(...)`, allocate the replacement, call `previewPipeline->resize(w, h)`, update the camera aspect, and rebind the image. Reject zero-sized dimensions, such as a minimized window. A fixed-size preview target only needs recreation when its own resolution changes.

On menu shutdown, stop rendering and detach GUI references before removing the target. Delete the owned pipeline and remove preview objects/cameras through the scene's normal owner. Do not delete scene-owned components independently. Render targets allocated through the driver have their own lifecycle; resource package cleanup does not replace explicit target release.

## 6. Blend gameplay cameras with CCameraBrain and Tween

Use separate target cameras for follow, overview, and cinematic viewpoints, with one output camera passed to the renderer. Target cameras can move through application controllers without rendering their own passes.

```cpp
CGameObject* followObject = zone->createEmptyObject();
CCamera* followCamera = followObject->addComponent<CCamera>();
followCamera->lookAt(core::vector3df(0.0f, 6.0f, -8.0f),
	core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);

CGameObject* overviewObject = zone->createEmptyObject();
CCamera* overviewCamera = overviewObject->addComponent<CCamera>();
overviewCamera->lookAt(core::vector3df(6.0f, 10.0f, -10.0f),
	core::vector3df(0.0f, 1.0f, 0.0f), Transform::Oy);

CGameObject* outputObject = zone->createEmptyObject();
CCamera* outputCamera = outputObject->addComponent<CCamera>();
outputCamera->setCullingMask(WorldLayer);
CCameraBrain* brain = outputObject->addComponent<CCameraBrain>();

// The brain resolves its sibling CCamera in updateComponent().
// Explicitly initialize it before requesting an immediate first pose.
brain->updateComponent();
brain->setTargetCamera(followCamera, 1.0f);
brain->lateUpdate();
```

Always initialize the first target with blend value `1.0f`. The second argument of `setTargetCamera` is the initial blend factor, not a duration. `0.0f` keeps the previous blended pose until something advances the factor. Render with `outputCamera`, never the target camera, to see the transition.

The brain registers with `CWorldTransformSystem` for late updates. Normal scene updates apply the blend after camera controllers update. Calling `lateUpdate()` explicitly is useful for initial setup or a deliberate immediate switch before a normal update; it does not replace regular scene updates.

### A controller owns the transition

The following methods belong to an application-defined `CViewpointController`. Its members are `CCameraBrain* m_brain` and `CTweenFloat* m_blendTween`, initialized to the live brain and `NULL`. Include `Tween/CTweenFloat.h` and `Tween/CTweenManager.h`.

```cpp
void CViewpointController::stopBlend()
{
	if (m_blendTween)
	{
		m_blendTween->OnUpdate = nullptr;
		m_blendTween->OnFinish = nullptr;
		m_blendTween->OnStop = nullptr;
		m_blendTween->stop();
		m_blendTween = NULL;
	}
}

void CViewpointController::switchCamera(CCamera* target, float durationMs)
{
	if (!target || !m_brain)
		return;

	stopBlend();
	m_brain->setTargetCamera(target, durationMs <= 0.0f ? 1.0f : 0.0f);

	if (durationMs <= 0.0f)
	{
		m_brain->lateUpdate();
		return;
	}

	m_blendTween = new CTweenFloat(0.0f, 1.0f, durationMs);
	m_blendTween->setEase(EEasingFunctions::EaseOutCubic);
	m_blendTween->setUseScaledTime(false);
	m_blendTween->OnUpdate = [this](CTween* tween)
	{
		m_brain->setBlendValue(static_cast<CTweenFloat*>(tween)->getValue());
	};
	m_blendTween->OnFinish = [this](CTween* tween)
	{
		m_brain->setBlendValue(1.0f);
		m_blendTween = NULL;
	};
	CTweenManager::getInstance()->addTween(m_blendTween);
}
```

For example, `switchCamera(overviewCamera, 1500.0f)` blends over 1.5 seconds, and `switchCamera(followCamera, 0.0f)` cuts immediately. Unscaled time keeps the transition moving during gameplay slow motion or a zero game time scale, provided the application's normal frame/scene updates still run. Use scaled time when the transition should follow gameplay time.

The normal engine update already advances the singleton `CTweenManager`; do not update that singleton again in the view. The manager deletes finished/stopped tweens. Treat `m_blendTween` as a non-owning pointer, and call `stopBlend()` before destroying the controller or any referenced scene/camera. Do not also delete a registered tween. Coordinate any external stop operation so the controller cannot retain a stale pointer.

Switching target snapshots the brain's current blended pose, so a new transition can start from an interrupted transition. The destination pose is sampled during each late update, allowing a moving follow target to remain live.

### Current implementation limits

The current [CCameraBrain.cpp](../Projects/Skylicht/Engine/Camera/CCameraBrain.cpp) interpolates position and look/up directions. It does not copy the target camera's culling mask, projection type, aspect, or orthographic scale. Configure those on the output camera. Use compatible perspective target cameras for this example; transition UI separately with its own OrthoUI camera.

The partial-blend code for FOV/near/far currently uses the stored blended fields rather than the newly sampled target values. It therefore does not provide the expected smooth lens interpolation; the target lens values are assigned when blend reaches 1. Keep lens settings equal across targets for predictable pose-only transitions, or implement and verify lens animation separately. This guide documents that limitation without modifying engine behavior.

## 7. Project a 3D position into 2D GUI coordinates

Use `CProjective::getScreenCoordinatesFrom3DPosition` from `Projective/CProjective.h` for a health bar, nameplate, or status indicator that follows a world object. This converts a world-space point into a GUI position; it does not render the object into an image.

Project with the same 3D camera that renders the world. When using `CCameraBrain`, pass its output camera, so the indicator follows the blended view. Use the current frame's camera matrices after object movement, camera controllers, and brain late update, before rendering the GUI. Projecting with stale matrices causes visible lag during camera transitions.

The following fragment assumes `worldPosition` is the target's current world-space position, `indicator` is a `CGUIElement*` directly under an untransformed canvas root, and `worldCamera` is valid. The UI projection and root coordinates must describe the same full-screen view:

```cpp
int width = (int)canvas->getRootElement()->getWidth();
int height = (int)canvas->getRootElement()->getHeight();
core::vector3df anchor = worldPosition + core::vector3df(0.0f, 2.0f, 0.0f);
float x = 0.0f;
float y = 0.0f;
bool show = false;
if (width > 0 && height > 0)
{
	bool projected = CProjective::getScreenCoordinatesFrom3DPosition(
		worldCamera, anchor, x, y, width, height);
	show = projected && x >= 0.0f && x < (float)width &&
		y >= 0.0f && y < (float)height;
}
indicator->setVisible(show);
if (show)
	indicator->setPosition(core::vector3df(x, y, 0.0f));
```

Choose the anchor offset from the object's bounds or an attachment point. Obtain a world-space position for parented objects rather than passing a local transform position. Configure the indicator's pivot/alignment so the desired part of the label sits on the projected anchor.

Important limits of the current helper:

- It applies the camera's projection and view matrices, then maps into the supplied width/height with a top-left origin and downward-positive Y.
- Its boolean result only checks whether clip-space W is nonnegative. It does not check the screen bounds, near/far clip planes, object layers, visibility, or occlusion. In particular, it is not a reliable front/behind test for orthographic cameras, whose W normally remains 1. Apply the required visibility/depth policy separately.
- Width/height describe the destination coordinate space. Supplying canvas-root dimensions directly works when that canvas maps across the same full camera view. Do not mix physical screen pixels with scaled canvas coordinates.
- An indicator under a translated, scaled, or rotated GUI parent needs conversion from the projected root coordinates into that parent's local coordinates before `setPosition`.
- For a camera rendered in a sub-viewport, project into that viewport's dimensions, then apply its offset and the screen-to-canvas mapping. Account for letterboxing or cropping as well.

For a 3D preview inside a GUI image, first project using the preview camera and the render target dimensions. If the complete texture is displayed without cropping in an axis-aligned image rectangle expressed in canvas-root coordinates, map the result into that rectangle:

```text
canvasX = imageLeft + projectedX / targetWidth * imageWidth
canvasY = imageTop  + projectedY / targetHeight * imageHeight
```

This assumes the displayed texture has the same upright orientation as the projection. Adjust the mapping for flipped UVs, source cropping, aspect-fit margins, or transformed GUI parents. Hide the label when the preview/target is unavailable, or when the point fails the application's clipping policy. Projection alone does not test whether the model is hidden behind other geometry.

Engine references: [CProjective.h](../Projects/Skylicht/Engine/Projective/CProjective.h) and [CProjective.cpp](../Projects/Skylicht/Engine/Projective/CProjective.cpp).

## 8. Check the result

- Confirm the world camera excludes preview objects and the preview camera excludes the world.
- Confirm the UI camera includes the canvas layer, including canvases loaded later.
- Check child models and generated instances for layer propagation.
- Check target dimensions, camera aspect, GUI layout, and projection after resize.
- Check transparent preview edges and orientation on the intended backend, with the actual pipeline and GUI shader.
- Switch viewpoints repeatedly during a blend, including slow motion and screen exit.
- Reopen the menu repeatedly and verify textures, camera objects, callbacks, and tweens do not accumulate.

## Engine source references

| Source | Behavior to inspect |
| --- | --- |
| [CCamera.h](../Projects/Skylicht/Engine/Camera/CCamera.h), [CCamera.cpp](../Projects/Skylicht/Engine/Camera/CCamera.cpp) | Projection, transform, aspect, clip planes, target size, and masks |
| [CGameObject.cpp](../Projects/Skylicht/Engine/GameObject/CGameObject.cpp), [CContainerObject.cpp](../Projects/Skylicht/Engine/GameObject/CContainerObject.cpp) | Layer assignment and child helpers |
| [CVisibleSystem.cpp](../Projects/Skylicht/Engine/Culling/CVisibleSystem.cpp), [CCullingSystem.cpp](../Projects/Skylicht/Engine/Culling/CCullingSystem.cpp) | Layer filtering and render culling |
| [CGraphics2D.cpp](../Projects/Skylicht/Engine/Graphics2D/CGraphics2D.cpp), [CGUIImage.cpp](../Projects/Skylicht/Engine/Graphics2D/GUI/CGUIImage.cpp) | Canvas filtering and texture composition |
| [CForwardRP.cpp](../Projects/Skylicht/Engine/RenderPipeline/CForwardRP.cpp), [CBaseRP.cpp](../Projects/Skylicht/Engine/RenderPipeline/CBaseRP.cpp) | Render target binding and camera setup |
| [CCameraBrain.h](../Projects/Skylicht/Engine/Camera/CCameraBrain.h), [CCameraBrain.cpp](../Projects/Skylicht/Engine/Camera/CCameraBrain.cpp) | Target snapshots and late-update blending |
| [Tween guide](../Projects/Skylicht/Engine/Tween/README.md), [CTweenManager.cpp](../Projects/Skylicht/Engine/Tween/CTweenManager.cpp) | Tween callbacks, updates, and ownership |
