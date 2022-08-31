#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleShader.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "Lightmapper/CLightmapper.h"
#include "CRotateComponent.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleShader* app = new SampleShader();
	getApplication()->registerAppEvent("SampleShader", app);
}

SampleShader::SampleShader() :
	m_scene(NULL),
	m_guiCamera(NULL),
	m_camera(NULL),
	m_bakeSHLighting(true),
	m_reflectionProbe(NULL),
	m_font(NULL),
	m_normalMap(NULL),
	m_dissolveRenderer(NULL),
	m_dissolveParam(0.0f)
{
	Lightmapper::CLightmapper::createGetInstance();
}

SampleShader::~SampleShader()
{
	// delete scene & gameobjects...
	delete m_scene;
	delete m_font;

	// delete clone material
	for (CMaterial* m : m_normalMapMats)
		m->drop();
	for (CMaterial* m : m_reflectionMats)
		m->drop();
	for (CMaterial* m : m_dissolveMats)
		m->drop();

	m_normalMapMats.clear();
	m_reflectionMats.clear();
	m_dissolveMats.clear();

	Lightmapper::CLightmapper::releaseInstance();

	delete m_forwardRP;
}

void SampleShader::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	// Load "BuiltIn.zip" to read files inside it
	io::IFileSystem* fs = app->getFileSystem();
	fs->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("Common.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SampleShader.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SampleModels.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath("SampleModelsResource.zip"), false, false);
	fs->addFileArchive(app->getBuiltInPath(app->getTexturePackageName("SampleModels").c_str()), false, false);

	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 100);

	// Load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// Create a Scene
	m_scene = new CScene();

	// Create a Zone in Scene
	CZone* zone = m_scene->createZone();

	// Create 2D camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// Create 3d camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// Sky
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/MonValley.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// 3D grid
	// CGameObject *grid = zone->createEmptyObject();
	// grid->addComponent<CGridPlane>();

	CMeshManager* meshManager = CMeshManager::getInstance();
	CEntityPrefab* prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Sponza/Textures");

	// reflection probe
	CGameObject* reflectionProbeObj = zone->createEmptyObject();
	m_reflectionProbe = reflectionProbeObj->addComponent<CReflectionProbe>();

	// load object model
	prefab = meshManager->loadModel("SampleModels/DamagedHelmet/DamagedHelmet.dae", NULL, true, false);
	if (prefab != NULL)
	{
		// load normal map
		m_normalMap = CTextureManager::getInstance()->getTexture("SampleModels/DamagedHelmet/Default_normal.jpg");

		// init material
		ArrayMaterial materials = CMaterialManager::getInstance()->initDefaultMaterial(prefab);

		initTestNormalMapShader(prefab, materials);
		initTestReflectionShader(prefab, materials);
		initTestDissoveShader(prefab, materials);
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleShader::initTestNormalMapShader(CEntityPrefab* prefab, ArrayMaterial& materials)
{
	for (CMaterial* material : materials)
	{
		CMaterial* newMaterial = material->clone();
		newMaterial->changeShader("SampleShader/Shader/Normal.xml");
		newMaterial->setUniformTexture("uTexNormal", m_normalMap);
		m_normalMapMats.push_back(newMaterial);
	}

	// create render mesh object
	CGameObject* object = m_scene->getZone(0)->createEmptyObject();

	// render mesh & init material
	CRenderMesh* renderer = object->addComponent<CRenderMesh>();
	renderer->initFromPrefab(prefab);
	renderer->initMaterial(m_normalMapMats);

	// set indirect lighting by baked SH
	CIndirectLighting* indirectLighting = object->addComponent<CIndirectLighting>();
	indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);

	// rotate
	CRotateComponent* rotate = object->addComponent<CRotateComponent>();
	rotate->setRotate(0.0f, 0.05f, 0.0f);

	m_objects.push_back(object);

	createCanvasText("Normal Map", core::vector3df(0.0f, 1.5f, 0.0f));
}

void SampleShader::initTestReflectionShader(CEntityPrefab* prefab, ArrayMaterial& materials)
{
	for (CMaterial* material : materials)
	{
		CMaterial* newMaterial = material->clone();
		newMaterial->changeShader("SampleShader/Shader/Reflection.xml");
		newMaterial->setUniformTexture("uTexNormal", m_normalMap);
		m_reflectionMats.push_back(newMaterial);
	}

	// create render mesh object
	CGameObject* object = m_scene->getZone(0)->createEmptyObject();

	// render mesh & init material
	CRenderMesh* renderer = object->addComponent<CRenderMesh>();
	renderer->initFromPrefab(prefab);
	renderer->initMaterial(m_reflectionMats);

	// set indirect lighting by baked SH
	CIndirectLighting* indirectLighting = object->addComponent<CIndirectLighting>();
	indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);

	// rotate
	CRotateComponent* rotate = object->addComponent<CRotateComponent>();
	rotate->setRotate(0.0f, 0.05f, 0.0f);

	// set position
	object->getTransformEuler()->setPosition(core::vector3df(3.0f, 0.0f, 0.0f));

	m_objects.push_back(object);

	createCanvasText("Reflection", core::vector3df(3.0f, 1.5f, 0.0f));
}

void SampleShader::initTestDissoveShader(CEntityPrefab* prefab, ArrayMaterial& materials)
{
	// create render mesh object
	CGameObject* object = m_scene->getZone(0)->createEmptyObject();

	// render mesh & init material
	CRenderMesh* renderer = object->addComponent<CRenderMesh>();
	renderer->initFromPrefab(prefab);

	// we need copy current mesh buffer and recompute color for dissolve shader
	std::vector<CRenderMeshData*>& renderers = renderer->getRenderers();
	for (CRenderMeshData* r : renderers)
	{
		CMesh* mesh = r->getMesh();

		const core::aabbox3df& bbox = mesh->getBoundingBox();

		// z up mesh
		float minY = bbox.MinEdge.Z;
		float maxY = bbox.MaxEdge.Z;

		float d = maxY - minY;
		if (d > 0.0f)
		{
			for (u32 i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
			{
				IMeshBuffer* mb = mesh->getMeshBuffer(i);

				IVertexBuffer* buffer = mb->getVertexBuffer();
				S3DVertexTangents* vtxBuffer = (S3DVertexTangents*)buffer->getVertices();

				CMeshBuffer<S3DVertexTangents>* newBuffer = new CMeshBuffer<S3DVertexTangents>(getVideoDriver()->getVertexDescriptor(EVT_TANGENTS));
				IVertexBuffer* newVtxBuffer = newBuffer->getVertexBuffer();

				for (int j = 0, m = buffer->getVertexCount(); j < m; j++)
				{
					S3DVertexTangents s = vtxBuffer[j];

					float f = (s.Pos.Z - minY) / d;
					s.Color.setRed((u32)(f * 255.0f));
					s.Color.setGreen(0);
					s.Color.setBlue(0);

					newVtxBuffer->addVertex(&s);
				}

				// set index buffer
				newBuffer->setIndexBuffer(mb->getIndexBuffer());

				// static buffer			
				newBuffer->setHardwareMappingHint(EHM_STATIC);

				// replace
				mesh->replaceMeshBuffer(i, newBuffer);

				// remove handle this buffer
				newBuffer->drop();
			}
		}
	}

	for (CMaterial* material : materials)
	{
		CMaterial* newMaterial = material->clone();
		newMaterial->changeShader("SampleShader/Shader/Dissolved.xml");
		newMaterial->setUniformTexture("uTexNormal", m_normalMap);
		m_dissolveMats.push_back(newMaterial);
	}

	renderer->initMaterial(m_dissolveMats);

	// set indirect lighting by baked SH
	CIndirectLighting* indirectLighting = object->addComponent<CIndirectLighting>();
	indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);

	// rotate
	CRotateComponent* rotate = object->addComponent<CRotateComponent>();
	rotate->setRotate(0.0f, 0.05f, 0.0f);

	// set position
	object->getTransformEuler()->setPosition(core::vector3df(6.0f, 0.0f, 0.0f));

	m_objects.push_back(object);

	m_dissolveRenderer = renderer;

	createCanvasText("Dissolved", core::vector3df(6.0f, 1.5f, 0.0f));
}

void SampleShader::updateDissoveShader()
{
	m_dissolveParam = m_dissolveParam + getTimeStep() * 0.001f;
	m_dissolveParam = fmodf(m_dissolveParam, 180.0f);

	// f [0 - 1]
	float f = fabsf(sinf(m_dissolveParam));

	// Update uniform uCutoff
	for (int i = 0, n = m_dissolveRenderer->getMaterialCount(); i < n; i++)
	{
		CMaterial* material = m_dissolveRenderer->getMaterial(i);
		CMaterial::SUniformValue* v = material->getUniform("uCutoff");

		// border color
		v->FloatValue[0] = 1.0f;
		v->FloatValue[1] = 0.0f;
		v->FloatValue[2] = 0.0f;

		// cutoff
		v->FloatValue[3] = f;

		material->updateShaderParams();
	}
}


void SampleShader::createCanvasText(const char* text, const core::vector3df& position)
{
	CGameObject* canvasObject = m_scene->getZone(0)->createEmptyObject();
	CCanvas* canvas = canvasObject->addComponent<CCanvas>();
	CGUIText* guiText = canvas->createText(core::rectf(0.0f, 0.0f, 700.0f, 100.0f), m_font);
	guiText->setTextAlign(CGUIElement::Center, CGUIElement::Middle);
	guiText->setText(text);
	guiText->setPosition(core::vector3df(-350.0f, 0.0f, 0.0f));

	CGUIElement* rootGUI = canvas->getRootElement();
	rootGUI->setScale(core::vector3df(-0.004f, -0.004f, 0.004f));
	rootGUI->setPosition(position);

	canvas->enable3DBillboard(true);
}

void SampleShader::onUpdate()
{
	// update shader
	updateDissoveShader();

	// update application
	m_scene->update();
}

void SampleShader::onRender()
{
	if (m_bakeSHLighting == true)
	{
		m_bakeSHLighting = false;

		for (CGameObject* obj : m_objects)
			obj->setVisible(false);

		CGameObject* bakeCameraObj = m_scene->getZone(0)->createEmptyObject();
		CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
		m_scene->updateAddRemoveObject();

		core::vector3df pos(0.0f, 0.0f, 0.0f);

		core::vector3df normal = CTransform::s_oy;
		core::vector3df tangent = CTransform::s_ox;
		core::vector3df binormal = normal.crossProduct(tangent);
		binormal.normalize();

		Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
		lm->initBaker(64);

		// compute sh
		Lightmapper::CSH9 sh = lm->bakeAtPosition(
			bakeCamera,
			m_forwardRP,
			m_scene->getEntityManager(),
			pos,
			normal, tangent, binormal);

		// bake environment
		m_reflectionProbe->bakeProbe(bakeCamera, m_forwardRP, m_scene->getEntityManager());

		// apply indirect lighting
		std::vector<CIndirectLighting*> lightings = m_scene->getZone(0)->getComponentsInChild<CIndirectLighting>(false);
		for (CIndirectLighting* indirect : lightings)
			indirect->setSH(sh.getValue());

		for (CGameObject* obj : m_objects)
			obj->setVisible(true);
	}

	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	CGraphics2D::getInstance()->render(m_camera);
}

void SampleShader::onPostRender()
{
	// post render application
}

bool SampleShader::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleShader::onResize(int w, int h)
{
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleShader::onResume()
{
	// resume application
}

void SampleShader::onPause()
{
	// pause application
}

void SampleShader::onQuitApp()
{
	// end application
	delete this;
}