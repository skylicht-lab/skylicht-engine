# Materials and shaders: authoring, runtime changes, and variants

This guide follows [SceneLoading.md](SceneLoading.md) and complements [CustomRenderSystem.md](CustomRenderSystem.md). It covers materials authored in Skylicht Editor, loading through `CMaterialManager`, temporary shader effects, texture mapping during shader changes, and preprocessing shader variants. Examples describe an independent application.

## 1. Asset responsibilities

| Asset/API | Responsibility |
| --- | --- |
| Editor-authored `.mat` | Named materials, selected shader, textures, uniforms, and render properties. This is the normal material-authoring workflow. |
| Shader `.xml` | Shader name, base render state, uniform bindings, texture resources, editor controls, stage paths, and optional instancing dependencies. |
| GLSL/HLSL stage sources | Vertex/pixel calculations and the vertex-input contract. |
| `CMaterialManager` | Load/cache/export/save material assets and initialize materials from prefabs. |
| `CMaterial` | One material's runtime parameters, textures, shader selection, and reference-counted lifetime. |
| `CRenderMesh` | Assign named materials to mesh buffers and participate in rendering. |

Use Skylicht Editor to choose the shader, assign textures, edit exposed properties, and save the `.mat`. Keep material names consistent with the model's material names. A material file may contain several materials; it is not necessarily one file per mesh slot. Package the `.mat`, shader XML, generated shader stages, and referenced textures together with the application assets.

The detailed XML reference is [Engine/Material/Shader/README.md](../Projects/Skylicht/Engine/Material/Shader/README.md). This guide explains the application workflow rather than duplicating its full uniform/type tables.

## 2. Load and assign a material asset

Include `Material/CMaterialManager.h` and `RenderMesh/CRenderMesh.h` through your normal application headers. After engine/shader initialization and mounting the resource archives:

```cpp
std::vector<std::string> textureFolders;
textureFolders.push_back("Models/Character/Textures");
ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial(
	"Models/Character/character.mat", true, textureFolders);
if (materials.empty())
{
	// Enter the application's asset-error path.
	return;
}

// renderMesh is an initialized CRenderMesh with a valid prefab/model.
renderMesh->initMaterial(materials, true);
```

`loadTexture = true` loads referenced textures; `textureFolders` provides additional lookup locations. The manager caches by filename and returns a reference to its array. Repeated loading is not an instruction to reread edited files. Do not retain the returned array reference beyond cache unload/release.

`initMaterial(materials, true)` clones the entries for this renderer. The default second argument is `false`, which shares references. Mesh assignment matches `CMaterial::getName()` to the mesh's material names, not simply array order. Verify names when a loaded material does not affect the intended buffer.

The manager also exposes `exportMaterial(prefab, path)`, `initDefaultMaterial(prefab)`, `createAndAddMaterial(array)`, and `saveMaterial(array, path)` for tools or procedural workflows. Follow the resulting array/material ownership in the implementation when using these APIs. Runtime visual effects normally modify private clones rather than resaving authored files.

Direct construction, `new CMaterial("Name", "Shaders/Effect.xml")`, is an additional option for procedural/custom rendering; it does not replace the Editor/.mat workflow.

## 3. Clone, change shader, and restore

For a temporary rim-light, hit flash, or highlight, prepare two material sets once: a normal set and an effect set cloned from it. Switch sets on state transitions; update only parameter values during animation. Avoid rebuilding or loading materials every frame.

The following fragments belong to an application effect controller with borrowed `CRenderMesh* m_renderer` and owned `ArrayMaterial m_defaults, m_effects`. Run setup once with empty arrays; release any previous owned references before reinitializing.

```cpp
// Capture private normal materials from the already configured renderer.
for (int i = 0; i < m_renderer->getMaterialCount(); ++i)
{
	CMaterial* source = m_renderer->getMaterial(i);
	if (!source)
		continue;

	CMaterial* normal = source->clone();
	m_defaults.push_back(normal);

	CMaterial* effect = normal->clone();
	effect->changeShader("Shaders/CharacterRim.xml");
	float rim[] = { 0.3f, 0.7f, 1.0f, 1.0f };
	effect->setUniform4("uRimColor", rim);
	effect->updateShaderParams();
	m_effects.push_back(effect);
}

// Normal state:
m_renderer->initMaterial(m_defaults);
// Enter effect state:
m_renderer->initMaterial(m_effects);
// Leave effect state:
m_renderer->initMaterial(m_defaults);
```

`CharacterRim.xml` and `uRimColor` are application assets/parameters, not built-in identifiers. Before committing the effect set, verify the intended shader loaded successfully and matches the model's vertex format, pipeline, textures, skinning, and instancing mode. `changeShader(path)` returns void and can leave the previous shader in place if loading fails; a non-null `getShader()` alone does not prove a switch succeeded. Preload/validate the intended shader through `CShaderManager::loadShader(path)` or `getShaderByPath(path)` and compare the resulting shader pointer after switching.

`changeShader()` saves the current material's extra parameters, initializes the new shader's parameter definitions, restores compatible saved parameters, and updates shader params. It does not make unrelated shader interfaces compatible. Retain distinct variants for cutout, transparent, skinned, and static meshes where their requirements differ.

Do not pass the renderer's own material array directly back into an operation that releases/replaces it. The separate arrays above own their references independently. `initMaterial()` grabs shared entries or creates clones, so the controller can keep both sets alive while the renderer switches between them.

When the controller finishes, release its own references:

```cpp
for (CMaterial* material : m_effects)
	material->drop();
m_effects.clear();
for (CMaterial* material : m_defaults)
	material->drop();
m_defaults.clear();
```

If restoring normal appearance during shutdown, do that while the renderer is still alive. A destructor must not dereference a renderer already destroyed by scene teardown. Dropping the controller's references alone is safe when the renderer still owns its assigned references. Keep texture/shader packages alive until all users are retired.

## 4. Change uniform values

Declare editable values as `MATERIAL_PARAM` uniforms in shader XML, with a unique compatible `valueIndex` and matching component count. Then set them by name and update shader params:

```cpp
float color[] = { 1.0f, 0.5f, 0.3f, 1.0f };
for (CMaterial* material : m_effects)
{
	material->setUniform4("uColor", color);
	material->updateShaderParams();
}
```

`setUniform`, `setUniform2`, `setUniform3`, and `setUniform4` update named CPU-side values; `updateShaderParams()` copies material parameters into their shader parameter slots. Batch several changes before calling it. These are material values, not arbitrary writes to every engine uniform: matrices, lights, camera state, and other callback-supplied types have their own binding paths.

`getUniform(name)` creates a record when missing, so it is not a shader-interface validation test. Inspect the shader's VS/FS uniform declarations and parameter index/type if a value does not appear. A name typo can otherwise look like a successful write with no visible effect. Reacquire parameter pointers after a shader change because initialization replaces parameter records.

## 5. UITexture is part of shader-switch texture mapping

Declare material texture inputs in `customUI`, including in both the normal and effect shaders that must exchange them:

```xml
<uniforms>
	<fs>
		<uniform name="uTexAlbedo" type="DEFAULT_VALUE" value="0" float="1" directX="false"/>
		<uniform name="uColor" type="MATERIAL_PARAM" valueIndex="1" value="1,1,1,1" float="4"/>
	</fs>
</uniforms>
<customUI>
	<ui control="UIGroup" name="Texture">
		<ui control="UITexture" name="uTexAlbedo" autoReplace="_albedo.;_A."/>
	</ui>
	<ui control="UIColor" name="uColor"/>
</customUI>
```

`UITexture` is necessary for the normal Editor-authored texture-mapping workflow through `changeShader()`, not merely a decorative editor control. `CMaterial::initMaterial()` visits these controls and `addShaderUI()` creates named texture records. `saveExtraParams()` saves the old shader's records; `reloadExtraParams()` maps them back by texture uniform name, using a saved shader-specific set when available or looking for compatible names in saved extras for a new variant.

Use the same semantic texture name, such as `uTexAlbedo`, across related shaders. Matching texture-slot numbers alone is insufficient for named texture restoration. If a variant uses different uniform names, explicitly remap/rebind those textures. Declare each transferable albedo/normal/packed/emissive input with its own `UITexture` in both interfaces. `autoReplace` supports texture filename discovery; it is not the shader-to-shader mapping key.

Restoration does not blindly overwrite textures already initialized on the destination: the current implementation skips destination records with a non-null texture. Shader resource defaults and saved per-shader overrides therefore affect the result. Validate A -> B -> A switches with real assets rather than assuming every texture is copied.

For an explicit named override:

```cpp
ITexture* texture = CTextureManager::getInstance()->getTexture("Textures/surface.png");
if (texture)
	material->setUniformTexture("uTexAlbedo", texture);
```

`setTexture(slot, texture)` binds by low-level slot, useful in procedural/custom renderers; it is not a substitute for the named texture records used by Editor material serialization and shader-switch restoration. `setUniformTexture()` associates the named texture. The destination shader sampler declaration must resolve to the intended slot. Keep texture lifetime/package ownership consistent with material use.

## 6. Shader XML contracts

A shader definition ties together stage code, uniforms, resources, UI, and optional instancing:

- `shaderConfig name` identifies the shader; `baseShader` selects the base draw/blend state such as `SOLID` or `TRANSPARENT_ALPHA_CHANNEL`.
- `uniforms/vs` and `uniforms/fs` define bindings for each stage. Use `matrix="true"` for matrices that require DirectX transposition. Sampler uniforms normally use `directX="false"`; HLSL binds texture resources differently.
- `MATERIAL_PARAM` uses `valueIndex` to select a material parameter slot. `WORLD_VIEW_PROJECTION`, `WORLD`, `VIEW_PROJECTION`, lighting, particle, and time types get values through engine shader callbacks.
- `resources` supplies textures such as a static lookup texture, reflection probe, shadow map, or RTT output. It is distinct from editable named material inputs.
- `customUI` exposes material controls and initializes their runtime records, including texture records needed during shader switching.
- `shader type="GLSL"` / `type="HLSL"` selects the backend stage files. Keep XML uniform order/layout compatible with the HLSL constant-buffer declarations and packing.
- `instancing` describes the expected base/instance vertex layout and instancing shader; `dependent` loads accompanying shader definitions. Calling `enableInstancing(true)` does not supply an absent instancing shader automatically.

Shader changes must preserve all required mesh attributes and rendering behavior. A static vertex shader cannot replace a skinning shader without providing the corresponding bone/transform path. Likewise ordinary mesh instancing and particle instancing use different descriptors. See the shader reference for `IShaderInstancing` and its registration hook.

`baseShader`, material depth/cull settings, and render-pipeline routing are separate concerns. An alpha value does not make a solid shader transparent. A forward shader must run in a compatible forward stage, and shadow/depth/emission passes may need their own supported variants. See [CustomRenderSystem.md](CustomRenderSystem.md) for explicit draw-state binding through `updateTexture()`, `CShaderMaterial::setMaterial()`, and driver draw calls.

## 7. Generate predefined shader variants with BuildShader.py

Shader source templates can contain includes and conditional code shared by many small variants. Describe each variant in XML using generated `vs`/`fs` outputs, optional `vs_source`/`fs_source` templates, and its `define` list:

```xml
<shader type="GLSL"
	vs="GLSL/SurfaceVS.glsl" fs="GLSL/SurfaceRimFS.glsl"
	vs_source="GLSL/SurfaceVS.d.glsl" fs_source="GLSL/SurfaceFS.d.glsl"
	define="RIM_LIGHT;USE_ALBEDO"/>
<shader type="HLSL"
	vs="HLSL/SurfaceVS.hlsl" fs="HLSL/SurfaceRimFS.hlsl"
	vs_source="HLSL/SurfaceVS.d.hlsl" fs_source="HLSL/SurfaceFS.d.hlsl"
	define="RIM_LIGHT;USE_ALBEDO"/>
```

Edit the templates and XML definitions, then generate the runtime stage files from the engine's Assets working directory:

```powershell
cd skylicht-engine/Assets
python BuildShader.py
```

The environment needs Python and the script's `pcpp` dependency installed. Use the project's approved environment; dependency installation is a separate setup action. Paths in each XML shader entry are resolved relative to that XML's directory. The script scans subdirectories below its current working directory for XML whose root is `shaderConfig`, and applies semicolon/comma-separated defines through the Python C preprocessor. For stages with both a source and output path, it expands/preprocesses the template and overwrites the output, adding a generated-file header. A stage without a source/output pair is left as authored.

This is preprocessing into GLSL/HLSL text, not offline compilation into GPU bytecode. Runtime shader loading still builds the backend shader. The generated stages are the files selected by `vs`/`fs`, so changing only a `.d.glsl` or `.d.hlsl` template without regenerating leaves the runtime using stale code. The runtime does not apply the XML define list as a substitute for this build step.

Give different variants distinct output paths whenever their expanded code differs. The script does not deduplicate or protect conflicting output names: two XML entries can overwrite the same file, with the later processed variant winning. Shared outputs are appropriate only when their generated content is intentionally identical.

The current full script also creates `../Bin` if needed and calls `BuildAsset.compress("BuiltIn")`; it can regenerate many stages and repack BuiltIn assets. Review the generated changes and package side effects before running it in an existing checkout. For a standalone application's shader folder, use the project's existing build integration or an explicitly scoped call to the script's `findShader(directory)` helper with correct working-directory/import setup, rather than inventing command-line flags: the script currently has no directory-filter CLI.

After generation, inspect only the intended XML/templates/outputs and copy/package the generated stages and XML through the application's normal asset pipeline. Do not edit a generated stage as the durable source of a variant. Regenerate whenever its template, includes, or define set changes, and verify both GLSL and HLSL outputs used by supported platforms.

## 8. Uniform buffers and advanced bindings

For a uniform block, declare it under `buffers` with `type="UNIFORM_BUFFER"`; its `valueIndex` selects a `CShaderManager::UBO` slot. Bind a compatible hardware constant buffer before drawing. Match block names, memory layout/alignment, and GLSL/HLSL declarations. Keep that buffer alive while the slot can be used, and clear or replace borrowed bindings before releasing it. This is separate from ordinary `MATERIAL_PARAM` values and should be introduced only when the shader requires it.

Use engine-managed callbacks/resources for standard camera, lighting, shadow, and particle data. Add a custom callback only for genuinely application-specific bindings, with explicit ownership and per-draw state management. The shader reference includes the full uniform/resource types and UBO examples.

## 9. Validation and lifetime

Check material names, shader-load logs, vertex descriptors, texture paths, pipeline support, and named UITexture entries before diagnosing shader arithmetic. Test normal -> effect -> normal, two instances with different colors, alpha/cutout behavior, skinned/instanced models, and each supported graphics backend. A private effect must not recolor every object sharing the cached original.

Manager cache entries are borrowed unless you take a reference or clone them. `clone()` returns an owned reference; release it with `drop()`. The renderer retains assigned references, while shader/texture managers and resource packages have separate lifetimes. Do not release global materials/textures/shaders while a loading scene, renderer, effect set, or shader callback still uses them. Runtime shader changes are not guaranteed hot reload of a cached file; use the supported reload tooling or a fresh application run for validation.

## Engine source references

- [Shader XML reference](../Projects/Skylicht/Engine/Material/Shader/README.md): schema, uniform/resource types, instancing and UBO layouts.
- [CMaterialManager.h](../Projects/Skylicht/Engine/Material/CMaterialManager.h): material loading, cache, export, creation and saving.
- [CMaterial.cpp](../Projects/Skylicht/Engine/Material/CMaterial.cpp): cloning, changeShader, UITexture registration, parameter restoration and draw state.
- [CRenderMesh.cpp](../Projects/Skylicht/Engine/RenderMesh/CRenderMesh.cpp) and [CRenderMeshData.cpp](../Projects/Skylicht/Engine/RenderMesh/CRenderMeshData.cpp): assignment, cloning and mesh material-name matching.
- [BuildShader.py](../Assets/BuildShader.py): preprocessing define variants and packaging side effects.
- [PBR.xml](../Assets/BuiltIn/Shader/PBR/Forward/PBR.xml): engine example with UITexture, material parameters, resources and source/generated stage paths.
