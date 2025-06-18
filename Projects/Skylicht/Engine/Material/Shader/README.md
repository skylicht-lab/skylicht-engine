Skylicht Engine Shader Information Structure
================

Let's take a look at an example shader file (.xml)

```xml
<shaderConfig name="TransparentColor" baseShader="TRANSPARENT_ALPHA_CHANNEL">
	<instancing vertex="standard" shader="TransparentInstancing" instancingVertex="standard_color"/>
	<dependent shader="TransparentInstancing.xml"/>
	<uniforms>
		<vs>
			<uniform name="uMvpMatrix" type="WORLD_VIEW_PROJECTION" value="0" float="16" matrix="true"/>
		</vs>
		<fs>
			<uniform name="uTexDiffuse" type="DEFAULT_VALUE" value="0" float="1" directX="false"/>
			<uniform name="uColor" type="MATERIAL_PARAM" valueIndex="1" value="1.0, 1.0, 1.0, 1.0" float="4"/>
		</fs>
	</uniforms>
	<customUI>
		<ui control="UIGroup" name="Texture">
			<ui control="UITexture" name="uTexDiffuse" autoReplace="_diff.tga"/>
			<ui control="UIColor" name="uColor"/>
		</ui>
	</customUI>
	<shader type="GLSL" vs="GLSL/TransparentColorVS.glsl" fs="GLSL/TransparentColorFS.glsl"/>
	<shader type="HLSL" vs="HLSL/TransparentColorVS.hlsl" fs="HLSL/TransparentColorFS.hlsl"/>
</shaderConfig>
```
Additionally, you can view sample shader files in the `Assets\BuiltIn\Shader` folder.

## shaderConfig
`<shaderConfig name="TransparentColor" baseShader="TRANSPARENT_ALPHA_CHANNEL">`
| Attribute | Description | Value |
|-----------|-------------|-------|
| name | This is the shader name | |
| baseShader | Description of inherited shader | SOLID<br>TRANSPARENT_ADD_COLOR<br>TRANSPARENT_MULTIPLY_COLOR<br>TRANSPARENT_SCREEN_COLOR<br>TRANSPARENT_ALPHA_CHANNEL |

## uniforms
`<uniform name="uColor" type="MATERIAL_PARAM" valueIndex="1" value="1.0, 1.0, 1.0, 1.0" float="4"/>`
| Attribute | Description | Value |
|-----------|-------------|-------|
| name | This is the Uniform name attached to the vertex shader or fragment shader | |
| type | The value passed to the uniform | VIEW_PROJECTION<br>WORLD_VIEW_PROJECTION<br>VIEW<br>WORLD<br>WORLD_INVERSE<br>WORLD_INVERSE_TRANSPOSE<br>WORLD_TRANSPOSE<br>BONE_MATRIX<br>BONE_COUNT<br>SHADOW_MAP_MATRIX<br>SHADOW_MAP_DISTANCE<br>SHADOW_BIAS<br>WORLD_CAMERA_POSITION<br>LIGHT_COLOR<br>LIGHT_AMBIENT<br>WORLD_LIGHT_DIRECTION<br>POINT_LIGHT_COLOR<br>POINT_LIGHT_POSITION<br>POINT_LIGHT_ATTENUATION<br>SPOT_LIGHT_COLOR<br>SPOT_LIGHT_DIRECTION<br>SPOT_LIGHT_POSITION<br>SPOT_LIGHT_ATTENUATION<br>MATERIAL_PARAM<br>DEFAULT_VALUE<br>SHADER_VEC2<br>SHADER_VEC3<br>SHADER_VEC4<br>SH_CONST<br>CUSTOM_VALUE<br>TEXTURE_MIPMAP_COUNT<br>TEXTURE_WIDTH_HEIGHT<br>DEFERRED_VIEW<br>DEFERRED_PROJECTION<br>DEFERRED_VIEW_PROJECTION<br>TIME_STEP<br>PARTICLE_VIEW_UP<br>PARTICLE_VIEW_LOOK<br>PARTICLE_ORIENTATION_UP<br>PARTICLE_ORIENTATION_NORMAL<br>LIGHTMAP_INDEX<br>TIME<br>COLOR_INTENSITY<br>RENDER_TEXTURE_MATRIX<br> |
| valueIndex| Is an input parameter for a value type | |
| value | Default value |
| float | Number of elements in the array | Example:<br>1 is float<br>4 is float4<br>16 is float4x4 |
| matrix| If a **uniform is a matrix**, this attribute is required for the Skylicht-Engine to transpose the matrix for DirectX |true|
| directX| **For uniform textures**, this property is needed because OpenGL and DirectX handle uniform binding a bit differently. DirectX doesn't require binding uniform textures |false|

The order of uniforms defined in the .xml shader file and the .hlsl shader file must be the same.

Example:
```xml
<vs>
	<uniform name="uMvpMatrix" type="WORLD_VIEW_PROJECTION" value="0" float="16" matrix="true"/>
	<uniform name="uWorldMatrix" type="WORLD" value="0" float="16" matrix="true"/>
	<uniform name="uCameraPosition" type="WORLD_CAMERA_POSITION" value="0" float="4"/>
	<uniform name="uUVScale" type="MATERIAL_PARAM" valueIndex="0" value="1.0, 1.0, 0.0, 0.0" float="4"/>
</vs>
```
So, in vertex shader:
```C
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uUVScale;
};
```


## ui
```xml
<ui control="UIGroup" name="Texture">
	<ui control="UITexture" name="uTexDiffuse" autoReplace="_diff.tga"/>
	<ui control="UIColor" name="uColor"/>
</ui>
```
Describes controls like sliders and images for adjusting uniform values in the Skylicht-Editor.
| Attribute | Description | Value |
|-----------|-------------|-------|
| control | Control name | UITexture<br>UIColor<br>UIFloat<br>UIFloat2<br>UIFloat3<br>UIFloat4<br>UIGroup
| name | Uniform name | |
| autoReplace | Only used for the UITexture control; it will automatically find textures with corresponding filenames |

## shader
```xml;
<shader type="GLSL" 
	vs="GLSL/SGShadowVS.glsl" 
	fs="GLSL/SGLitShadowFS.glsl" 
	vs_source="GLSL/SGVS.d.glsl" 
	fs_source="GLSL/SGLitFS.d.glsl" 
	define="SHADOW;OPTIMIZE_SHADOW"/>

<shader type="HLSL" 
	vs="HLSL/SGShadowVS.hlsl"
	fs="HLSL/SGLitShadowFS.hlsl"
	vs_source="HLSL/SGVS.d.hlsl"
	fs_source="HLSL/SGLitFS.d.hlsl"
	define="SHADOW;OPTIMIZE_SHADOW"/>
```
| Attribute | Description | Value |
|-----------|-------------|-------|
| type | | GLSL, HLSL |
| vs | Path to the vertex shader file |
| fs | Path to the fragment shader file |
| vs_source<br>fs_source | Path to the source file; it will be compiled by the `python BuildShader.py` command.<br>The compiled output will be overwritten to the vs or fs path |
| define | Definition flags during compilation | You can refer to the sample file `BuiltIn\Shader\PBR\Forward\PBR.xml`|

## instancing
`<instancing vertex="standard" shader="TransparentInstancing" instancingVertex="standard_color"/>`

Instancing information is described when this shader supports instancing. It will be passed to the instancing shader when the `CRenderMesh::enableInstancing` method is called.

| Attribute | Description | Value |
|-----------|-------------|-------|
| vertex | Description of vertex input data |standard<br>2tcoords<br>tangents<br>skin<br>skintangents<br>2tcoordstangents<br>skin2tcoordtangents|
| shader | The name of shader instancing | |
| instancingVertex | Detailed information on batched vertex instancing structure | standard_color<br>2texcoords_color<br>standard_sg<br>tangents_sg |

You can read more of the source code for batching instancing here `Projects\Skylicht\Engine\Material\Shader\Instancing`

You can also refer to some instancing-supported shaders within the engine:
- `BuiltIn\Shader\Mobile\MobileSG.xml`
- `BuiltIn\Shader\Toon\Toon.xml`
- `BuiltIn\Shader\SpecularGlossiness\Deferred\Color.xml`

## dependent
Defines an accompanying shader file (.xml) to be loaded, typically its own shader instancing.