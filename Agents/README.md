# Skylicht application guides

Read these guides in order when creating an application with Skylicht Engine. Examples describe an independent application and require no private game repository, assets, or services.

| Order | Guide | Summary |
| --- | --- | --- |
| 1 | [Setup.md](Setup.md) | Create a standalone SDK consumer: project layout, CMake, platform entry points, assets, and optional editor integration. |
| 2 | [Architecture.md](Architecture.md) | Understand scene ownership, GameObjects and components, entity data, systems, and rendering responsibilities. |
| 3 | [FirstInit.md](FirstInit.md) | Present the first loading frame, initialize in stages, and manage resource packages across menu/world transitions. |
| 4 | [Camera.md](Camera.md) | Set up 2D/3D cameras, filter objects with layers and masks, compose a transparent 3D preview into GUI, project world positions into 2D indicators, and blend gameplay cameras with a brain and tweens. |
| 5 | [Localize.md](Localize.md) | Load translations with public SampleFW CLocalize, bind GUI localization callbacks, refresh open screens, and select fonts for different languages. |

Start with the [engine README](../README.md) for engine build requirements. Source links in these guides refer to this checkout; verify API behavior again when upgrading the engine. Code fragments illustrate integration points rather than complete applications.
