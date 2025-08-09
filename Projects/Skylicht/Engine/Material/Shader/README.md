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
| type | The value passed to the uniform |See next table|
| valueIndex| Is an input parameter for a value type | |
| value | Default value ||
| float | Number of elements in the array | Example:<br>1 is float<br>4 is float4<br>16 is float4x4 |
| matrix| If a **uniform is a matrix**, this attribute is required for the Skylicht-Engine to transpose the matrix for DirectX |true|
| directX| When the directX=false attribute is set, this uniform will be ignored if the engine is using DirectX. **For uniform textures**, this property is needed because OpenGL and DirectX handle uniform binding a bit differently. DirectX doesn't require binding uniform textures |false|

Value table for the `type` property
| Type | Value type | Description |
|------|------------|-------------|
|VIEW|float4x4| Model-to-camera transformation matrix.<br> `viewMatrix = getVideoDriver()->getTransform(video::ETS_VIEW)`|
|WORLD|float4x4| Model transformation matrix.<br> `worldMatrix = getVideoDriver()->getTransform(video::ETS_WORLD)`|
|VIEW_PROJECTION|float4x4| It's often used when rendering with instancing, because world matrices are batched in the vertex attributes<br> `vpMatrix = projectionMatrix * viewMatrix`|
|WORLD_VIEW_PROJECTION|float4x4| The MVP transformation matrix to convert vertex coordinates from 3D to 2D.<br> `mvpMatrix = projectionMatrix x viewMatrix x worldMatrix`|
|WORLD_INVERSE|float4x4| Matrix to transform vertices from world coordinates to local coordinates, where local coordinates are the coordinates within the 3D model file.|
|WORLD_TRANSPOSE|float4x4| |
|BONE_MATRIX|float4x4[64]| Matrices to transform skin-vertices to the animated skeleton's positions.<br>See `BuiltIn/Shader/Toon/SkinToon.xml`|
|BONE_COUNT|float2| |
|SHADOW_MAP_MATRIX|float4x4[4]| Matrices to project 3D vertex coordinates onto the 2D cascade shadow map textures|
|SHADOW_MAP_DISTANCE|float4| The maximum depth (in meters) covered by the cascade shadow map|
|SHADOW_BIAS|float4| |
|WORLD_CAMERA_POSITION|float4| The camera position|
|LIGHT_COLOR|float4| |
|LIGHT_AMBIENT|float4| |
|WORLD_LIGHT_DIRECTION|float4| It returns the light direction vector for the directional light, similar to how you'd consider the direction of sunlight.|
|POINT_LIGHT_COLOR|float4| |
|POINT_LIGHT_POSITION|float4| The light position, See `CShaderLighting::setPointLight`|
|POINT_LIGHT_ATTENUATION|float4| `value.y = CLight::getAttenuation()`<br>See `BuiltIn\Shader\SpecularGlossiness\Lighting\SGPointLight.xml`|
|SPOT_LIGHT_COLOR|float4| |
|SPOT_LIGHT_DIRECTION|float4| |
|SPOT_LIGHT_POSITION|float4| The light position, See `CShaderLighting::setSpotLight`|
|SPOT_LIGHT_ATTENUATION|float4| `value.x = cos(CSpotLight::getSplotCutoff() * 0.5f)`<br> `value.y = cos(CSpotLight::getSpotInnerCutof() * 0.5f)`<br> `value.z = CLight::getAttenuation()`<br> `value.w = CSpotLight::getSpotExponent()`<br>See `BuiltIn\Shader\SpecularGlossiness\Lighting\SGSpotLight.xml`|
|MATERIAL_PARAM|float2<br>float3<br>float4|Gets the value already set at *valueIndex* within *CMaterial*.<br>See `CMaterial::setUniform, CMaterial::setUniform2, CMaterial::setUniform3, CMaterial::setUniform4, CMaterial::updateShaderParams`<br><br>Sample project:<br>- Code: `Samples\Shader\Source\SampleShader.cpp`<br>- Shader: `Assets\SampleShader\Shader\Dissolved.xml`
|DEFAULT_VALUE|| |
|SHADER_VEC2|float2| |
|SHADER_VEC3|float3| |
|SHADER_VEC4|float4| |
|SH_CONST|float4[4]| The value used to calculate ambient light color based on Spherical Harmonics (SH)<br>See `CIndirectLighting`|
|CUSTOM_VALUE|| |
|TEXTURE_MIPMAP_COUNT|float2| |
|TEXTURE_WIDTH_HEIGHT|float2| |
|DEFERRED_VIEW|float4x4| |
|DEFERRED_PROJECTION|float4x4| |
|DEFERRED_VIEW_PROJECTION|float4x4| |
|TIME_STEP|float2| `value.x = getTimeStep()`|
|PARTICLE_VIEW_UP|float4| *Used in the particle component*|
|PARTICLE_VIEW_LOOK|float4| *Used in the particle component*|
|PARTICLE_ORIENTATION_UP|float4| *Used in the particle component*|
|PARTICLE_ORIENTATION_NORMAL|float4| *Used in the particle component*|
|LIGHTMAP_INDEX|float2| *Used in the lightmapper to bake light color to texture*|
|TIME|float4| `value.x = time()`<br> `value.y = time() * 2.0`<br>`value.z = time() * 3.0`<br>`value.w = time() * 4.0`<br>Default, The return value of time() will loop from 0.0 to 4.0. However, you can also adjust the default value by setting the attribute `value="8.0"` in the **uniform** node.<br>See `BuiltIn\Shader\BasicVfx\VfxScrollUV1.xml`|
|COLOR_INTENSITY|float4| *Used in the particle component.*<br>This is the return value of the `CShaderMaterial::getColorIntensity()` function. It's used in the shader to boost color brightness beyond the 255-byte limit.<br>Call the `CShaderMaterial::setColorIntensity(const SColorf& c)` function to set the value before drawing.|
|RENDER_TEXTURE_MATRIX|float4x4| Matrix to project 3D vertex coordinates onto a 2D render texture, which is the result of CRenderToTextureRP|

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
## resources
```xml
<resources>
	<resource name="uTexReflect" type="ReflectionProbe"/>
	<resource name="uTexBRDF" type="Texture" path="BuiltIn/Textures/BRDF.png"/>
</resources>
```
Set the default texture for the uniform.

| Attribute | Description | Value |
|-----------|-------------|-------|
|name|The uniform name||
|type|See next table||
|path|Path to the texture file (if type is Texture or CubeTexture)||

Value table for the `type` property
| Type | Description |
|------|-------------|
|Texture|Specify the path to the texture file.|
|CubeTexture|Specify the path to the cube texture file|
|ReflectionProbe|Nearest reflection probe's reflection texture `TextureCube`|
|ShadowMap|Depth shadow map `Texture2DArray`|
|TransformTexture|`CShaderTransformTexture::getTexture()`|
|VertexPositionTexture|`CShaderTransformTexture::getPositionTexture()`|
|VertexNormalTexture|`CShaderTransformTexture::getNormalTexture()`|
||TransformTexture, VertexPositionTexture, VertexNormalTexture used for instanced rendering of skin meshes. <br>See example: `Samples\BoidSystem` and `Samples\BoidSystemVAT`|
|LastFrame|Used for Screen-Space-Reflection|
|RTT0|Get the target texture from the Render-to-Texture Pipeline `CRenderToTextureRP`|
|RTT1||
|RTT2||
|RTT3||
|RTT4||
|RTT5||
|RTT6||
|RTT7||

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
| control | Control name | UITexture<br>UIColor<br>UIFloat<br>UIFloat2<br>UIFloat3<br>UIFloat4<br>UIGroup |
| name | Uniform name | |
| autoReplace | Only used for the UITexture control; it will automatically find textures with corresponding filenames | |

## shader
```xml
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
| vs | Path to the vertex shader file | |
| fs | Path to the fragment shader file | |
| vs_source<br>fs_source | Path to the source file; it will be compiled by the `python BuildShader.py` command.<br>The compiled output will be overwritten to the vs or fs path | |
| define | Definition flags during compilation | You can refer to the sample file `BuiltIn\Shader\PBR\Forward\PBR.xml`|

## instancing
`<instancing vertex="standard" shader="TransparentInstancing" instancingVertex="standard_color"/>`

Instancing information is described when this shader supports instancing. It will be passed to the instancing shader when the `CRenderMesh::enableInstancing` method is called.

| Attribute | Description | Value |
|-----------|-------------|-------|
| vertex | Description of vertex input data |standard<br>2tcoords<br>tangents<br>skin<br>skintangents<br>2tcoordstangents<br>skin2tcoordtangents|
| shader | The name of shader instancing | |
| instancingVertex | Detailed information on batched vertex instancing structure | standard_color<br>2texcoords_color<br>standard_sg<br>tangents_sg |

Example: **vertex=standard**
- `vertex="standard" instancingVertex="standard_color"`

Here's how instanced vertices are batched (see `CStandardColorInstancing`)

```C
struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float4 uUVScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;

	float4x4 worldMatrix: TEXCOORD3;
};
```

- `vertex="standard" instancingVertex="tangents_sg"`

Here's how instanced vertices are batched (see `CStandardSGInstancing`)

```C
struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;

	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4 uSpecGloss: TEXCOORD3;

	float4x4 worldMatrix: TEXCOORD4;
}
```

You can define your own batching structure.

```C
class CYourBatchingInstancing : public IShaderInstancing
{
public:
	CYourBatchingInstancing();

	virtual ~CYourBatchingInstancing();

	virtual IVertexBuffer* createInstancingVertexBuffer();

	virtual IMeshBuffer* createMeshBuffer(video::E_INDEX_TYPE type);

	// The function performs the batching process from multiple Entities and Materials.
	virtual void batchIntancing(IVertexBuffer* vtxBuffer,
		CMaterial** materials,
		CEntity** entities,
		int count);
};


CShaderManager *shaderMgr = CShaderManager::getInstance();
shaderMgr->OnCreateInstancingVertex = [](const char* vertexType)
{
	if (std::string(vertexType) == "YourVertexTypeName")
	{
		return new CYourBatchingInstancing();
	}
	return NULL;
};
```

You can read more of the source code for batching instancing here `Projects\Skylicht\Engine\Material\Shader\Instancing`

You can also refer to some instancing-supported shaders within the engine:
- `BuiltIn\Shader\Mobile\MobileSG.xml`
- `BuiltIn\Shader\Toon\Toon.xml`
- `BuiltIn\Shader\SpecularGlossiness\Deferred\Color.xml`

## dependent
Defines an accompanying shader file (.xml) to be loaded, typically its own shader instancing.