// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "IrrCompileConfig.h"
#include "CSceneManager.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IXMLWriter.h"
#include "IMaterialRenderer.h"
#include "IReadFile.h"
#include "IWriteFile.h"

#include "irrOS.h"

#include "CEmptySceneNode.h"
#include "CCameraSceneNode.h"
#include "CSceneCollisionManager.h"
#include "CTriangleSelector.h"
#include "COctreeTriangleSelector.h"
#include "CTriangleBBSelector.h"
#include "CMetaTriangleSelector.h"

#include "CGeometryCreator.h"
#include "CMeshManipulator.h"

//! Enable debug features
// #define SCENEMANAGER_DEBUG

namespace irr
{
namespace scene
{

//! constructor
CSceneManager::CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs,
		gui::ICursorControl* cursorControl, IMeshCache* cache,
		gui::IGUIEnvironment* gui)
: ISceneNode(0, 0), Driver(driver), FileSystem(fs), GUIEnvironment(gui),
	CursorControl(cursorControl), CollisionManager(0),
	ActiveCamera(0), ShadowColor(150,0,0,0), AmbientLight(0,0,0,0), Parameters(0),
	MeshCache(cache), CurrentRendertime(ESNRP_NONE),
	IRR_XML_FORMAT_SCENE(L"irr_scene"), IRR_XML_FORMAT_NODE(L"node"), IRR_XML_FORMAT_NODE_ATTR_TYPE(L"type")
{
	#ifdef _DEBUG
	ISceneManager::setDebugName("CSceneManager ISceneManager");
	ISceneNode::setDebugName("CSceneManager ISceneNode");
	#endif

	// root node's scene manager
	SceneManager = this;

	if (Driver)
		Driver->grab();

	if (FileSystem)
		FileSystem->grab();

	if (CursorControl)
		CursorControl->grab();

	// set scene parameters
	Parameters = new io::CAttributes();
	Parameters->setAttribute(DEBUG_NORMAL_LENGTH, 1.f);
	Parameters->setAttribute(DEBUG_NORMAL_COLOR, video::SColor(255, 34, 221, 221));

	// create collision manager
	CollisionManager = new CSceneCollisionManager(this, Driver);

	// create mesh
	MeshManipulator = new CMeshManipulator();

	// create geometry creator
	GeometryCreator = new CGeometryCreator(Driver);
}


//! destructor
CSceneManager::~CSceneManager()
{
	clearDeletionList();

	if (FileSystem)
		FileSystem->drop();

	if (CursorControl)
		CursorControl->drop();

	if (CollisionManager)
		CollisionManager->drop();

	if (MeshManipulator)
		MeshManipulator->drop();

	if (GeometryCreator)
		GeometryCreator->drop();

	if (ActiveCamera)
		ActiveCamera->drop();
	ActiveCamera = 0;

	if (Parameters)
		Parameters->drop();

	// remove all nodes and animators before dropping the driver
	// as render targets may be destroyed twice

	removeAll();
	removeAnimators();

	if (Driver)
		Driver->drop();
}

//! returns the video driver
video::IVideoDriver* CSceneManager::getVideoDriver()
{
	return Driver;
}


//! returns the GUI Environment
gui::IGUIEnvironment* CSceneManager::getGUIEnvironment()
{
	return GUIEnvironment;
}

//! Get the active FileSystem
/** \return Pointer to the FileSystem
This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
io::IFileSystem* CSceneManager::getFileSystem()
{
	return FileSystem;
}


//! Adds a camera scene node to the tree and sets it as active camera.
//! \param position: Position of the space relative to its parent where the camera will be placed.
//! \param lookat: Position where the camera will look at. Also known as target.
//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
//! the camera will move too.
//! \return Returns pointer to interface to camera
ICameraSceneNode* CSceneManager::addCameraSceneNode(ISceneNode* parent,
	const core::vector3df& position, const core::vector3df& lookat, s32 id,
	bool makeActive)
{
	return NULL;
}

//! Adds an empty scene node.
ISceneNode* CSceneManager::addEmptySceneNode(ISceneNode* parent, s32 id)
{
	if (!parent)
		parent = this;

	ISceneNode* node = new CEmptySceneNode(parent, this, id);
	node->drop();

	return node;
}

//! Returns the root scene node. This is the scene node wich is parent
//! of all scene nodes. The root scene node is a special scene node which
//! only exists to manage all scene nodes. It is not rendered and cannot
//! be removed from the scene.
//! \return Returns a pointer to the root scene node.
ISceneNode* CSceneManager::getRootSceneNode()
{
	return this;
}


//! Returns the current active camera.
//! \return The active camera is returned. Note that this can be NULL, if there
//! was no camera created yet.
ICameraSceneNode* CSceneManager::getActiveCamera() const
{
	return ActiveCamera;
}


//! Sets the active camera. The previous active camera will be deactivated.
//! \param camera: The new camera which should be active.
void CSceneManager::setActiveCamera(ICameraSceneNode* camera)
{
	if (camera)
		camera->grab();
	if (ActiveCamera)
		ActiveCamera->drop();

	ActiveCamera = camera;
}

//! renders the node.
void CSceneManager::render()
{
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSceneManager::getBoundingBox() const
{
	_IRR_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager wanted.

	// should never be used.
	return *((core::aabbox3d<f32>*)0);
}


//! returns if node is culled
bool CSceneManager::isCulled(const ISceneNode* node) const
{
	const ICameraSceneNode* cam = getActiveCamera();
	if (!cam)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}
	bool result = false;

	// has occlusion query information
	if (node->getAutomaticCulling() & scene::EAC_OCC_QUERY)
	{
		result = (Driver->getOcclusionQueryResult(const_cast<ISceneNode*>(node))==0);
	}

	// can be seen by a bounding box ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_BOX))
	{
		core::aabbox3d<f32> tbox = node->getBoundingBox();
		node->getAbsoluteTransformation().transformBoxEx(tbox);
		result = !(tbox.intersectsWithBox(cam->getViewFrustum()->getBoundingBox() ));
	}

	// can be seen by a bounding sphere
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_SPHERE))
	{ // requires bbox diameter
	}

	// can be seen by cam pyramid planes ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_BOX))
	{
		SViewFrustum frust = *cam->getViewFrustum();

		//transform the frustum to the node's current absolute transformation
		core::matrix4 invTrans(node->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
		//invTrans.makeInverse();
		frust.transform(invTrans);

		core::vector3df edges[8];
		node->getBoundingBox().getEdges(edges);

		for (s32 i=0; i<scene::SViewFrustum::VF_PLANE_COUNT; ++i)
		{
			bool boxInFrustum=false;
			for (u32 j=0; j<8; ++j)
			{
				if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
				{
					boxInFrustum=true;
					break;
				}
			}

			if (!boxInFrustum)
			{
				result = true;
				break;
			}
		}
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return result;
}


//! registers a node for rendering it at a specific time.
u32 CSceneManager::registerNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass)
{
	u32 taken = 0;

	switch(pass)
	{
		// take camera if it is not already registered
	case ESNRP_CAMERA:
		{
			taken = 1;
			for (u32 i = 0; i != CameraList.size(); ++i)
			{
				if (CameraList[i] == node)
				{
					taken = 0;
					break;
				}
			}
			if (taken)
			{
				CameraList.push_back(node);
			}
		}
		break;

	case ESNRP_LIGHT:
		// TODO: Point Light culling..
		// Lighting model in irrlicht has to be redone..
		//if (!isCulled(node))
		{
			LightList.push_back(node);
			taken = 1;
		}
		break;

	case ESNRP_SKY_BOX:
		SkyBoxList.push_back(node);
		taken = 1;
		break;
	case ESNRP_CUBE_REFLECT:
		if (!isCulled(node))
		{
			CubeReflectList.push_back(node);
			taken = 1;
		}
		break;
	case ESNRP_SOLID:
		if (!isCulled(node))
		{
			SolidNodeList.push_back(node);
			taken = 1;
		}
		break;	
	case ESNRP_TRANSPARENT:
		if (!isCulled(node))
		{
			TransparentNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT_EFFECT:
		if (!isCulled(node))
		{
			TransparentEffectNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_AUTOMATIC:
		if (!isCulled(node))
		{
			const u32 count = node->getMaterialCount();

			taken = 0;
			for (u32 i=0; i<count; ++i)
			{				
				if (Driver->isMaterialTransparent(node->getMaterial(i)) == true)
				{
					// register as transparent node
					TransparentNodeEntry e(node, camWorldPos);
					TransparentNodeList.push_back(e);
					taken = 1;
					break;
				}
			}

			// not transparent, register as solid
			if (!taken)
			{
				SolidNodeList.push_back(node);
				taken = 1;
			}
		}
		break;
	case ESNRP_SHADOW:
		ShadowNodeList.push_back(node);
		taken = 1;
		break;

	case ESNRP_NONE: // ignore this one
		break;
	}

#ifdef SCENEMANAGER_DEBUG
	s32 index = Parameters->findAttribute("calls");
	Parameters->setAttribute(index, Parameters->getAttributeAsInt(index)+1);

	if (!taken)
	{
		index = Parameters->findAttribute("culled");
		Parameters->setAttribute(index, Parameters->getAttributeAsInt(index)+1);
	}
#endif

	return taken;
}

void CSceneManager::update()
{
	if (!Driver)
		return;

	// do animations and other stuff.
	OnAnimate(os::Timer::getTime());

	/*!
		First Scene Node for prerendering should be the active camera
		consistent Camera is needed for culling
	*/
	camWorldPos.set(0,0,0);
	if (ActiveCamera)
	{
		ActiveCamera->render();
		camWorldPos = ActiveCamera->getAbsolutePosition();
	}

	CameraList.set_used(0);
	SkyBoxList.set_used(0);			
	SolidNodeList.set_used(0);
	TransparentNodeList.set_used(0);
	TransparentEffectNodeList.set_used(0);
	ShadowNodeList.set_used(0);
	LightList.set_used(0);
	CubeReflectList.set_used(0);

	// todo
	/*
	int checkCount = 0;
	ISceneNodeList::Iterator it = Children.begin(), end = Children.end();
	for (; it != end; ++it)
	{
		(*it)->CheckCount(checkCount);
	}
	printf("Num sceneNode: %d\n", checkCount);
	*/

	// let all nodes register themselves
	OnRegisterSceneNode();

	// Sort the lights by distance from the camera
	core::vector3df camWorldPos(0, 0, 0);
	if (ActiveCamera)
		camWorldPos = ActiveCamera->getAbsolutePosition();

	core::array<DistanceNodeEntry> SortedLights;
	SortedLights.set_used(LightList.size());
	for (s32 light = (s32)LightList.size() - 1; light >= 0; --light)
		SortedLights[light].setNodeAndDistanceFromPosition(LightList[light], camWorldPos);

	SortedLights.set_sorted(false);
	SortedLights.sort();

	for(s32 light = (s32)LightList.size() - 1; light >= 0; --light)
		LightList[light] = SortedLights[light].Node;

	Driver->deleteAllDynamicLights();
	Driver->setAmbientLight(AmbientLight);

	CubeReflectList.sort(); // sort by textures
	SolidNodeList.sort(); // sort by textures
	TransparentNodeList.sort(); // sort by distance from camera
	TransparentEffectNodeList.sort(); // sort by distance from camera	

	clearDeletionList();
}

void CSceneManager::draw()
{
	if (!Driver)
		return;

	// reset attributes
	Parameters->setAttribute("culled", 0);
	Parameters->setAttribute("calls", 0);
	Parameters->setAttribute("drawn_solid", 0);
	Parameters->setAttribute("drawn_transparent", 0);
	Parameters->setAttribute("drawn_transparent_effect", 0);

	u32 i; // new ISO for scoping problem in some compilers

	// reset all transforms
	Driver->setMaterial(video::SMaterial());
	Driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );	
	Driver->setAllowZWriteOnTransparent(Parameters->getAttributeAsBool(ALLOW_ZWRITE_ON_TRANSPARENT));

	//render camera scenes
	{
		CurrentRendertime = ESNRP_CAMERA;

		//for (i=0; i<CameraList.size(); ++i)
		//	CameraList[i]->render();

		if (ActiveCamera != NULL)
			ActiveCamera->render();
	}

	//render lights scenes
	{
		CurrentRendertime = ESNRP_LIGHT;		
		u32 maxLights = LightList.size();

		for (i=0; i< maxLights; ++i)
			LightList[i]->render();
	}

	// render skyboxes
	{
		CurrentRendertime = ESNRP_SKY_BOX;

		for (i=0; i<SkyBoxList.size(); ++i)
			SkyBoxList[i]->render();		
	}


	// render default objects
	{
		CurrentRendertime = ESNRP_SOLID;

		for (i=0; i<SolidNodeList.size(); ++i)
			SolidNodeList[i].Node->render();

		Parameters->setAttribute("drawn_solid", (s32) SolidNodeList.size());		
	}

	// render transparent objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT;

		for (i=0; i<TransparentNodeList.size(); ++i)
			TransparentNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent", (s32) TransparentNodeList.size());
	}

	// render transparent effect objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT_EFFECT;

		for (i=0; i<TransparentEffectNodeList.size(); ++i)
			TransparentEffectNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent_effect", (s32) TransparentEffectNodeList.size());
	}	

	CurrentRendertime = ESNRP_NONE;
}

void CSceneManager::draw(E_SCENE_NODE_RENDER_PASS renderPass)
{
	if (!Driver)
		return;

	// reset attributes
	Parameters->setAttribute("culled", 0);
	Parameters->setAttribute("calls", 0);
	Parameters->setAttribute("drawn_solid", 0);
	Parameters->setAttribute("drawn_transparent", 0);
	Parameters->setAttribute("drawn_transparent_effect", 0);

	u32 i; // new ISO for scoping problem in some compilers

	// reset all transforms
	Driver->setMaterial(video::SMaterial());
	Driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );	
	Driver->setAllowZWriteOnTransparent(Parameters->getAttributeAsBool(ALLOW_ZWRITE_ON_TRANSPARENT));

	//render camera scenes
	camWorldPos.set(0,0,0);
	if (ActiveCamera)
	{
		ActiveCamera->render();
		camWorldPos = ActiveCamera->getAbsolutePosition();
	}

	//render lights scenes
	{
		CurrentRendertime = ESNRP_LIGHT;		
		u32 maxLights = LightList.size();

		for (i=0; i< maxLights; ++i)
			LightList[i]->render();
	}

	// render skyboxes
	if (renderPass == ESNRP_SKY_BOX || renderPass == ESNRP_CUBE_REFLECT)
	{
		CurrentRendertime = ESNRP_SKY_BOX;

		for (i=0; i<SkyBoxList.size(); ++i)
			SkyBoxList[i]->render();		
	}

	if (renderPass == ESNRP_CUBE_REFLECT)
	{
		CurrentRendertime = ESNRP_CUBE_REFLECT;

		for (i=0; i<CubeReflectList.size(); ++i)
			CubeReflectList[i].Node->render();		
	}

	// render default objects
	if (renderPass == ESNRP_SOLID)
	{
		CurrentRendertime = ESNRP_SOLID;

		for (i=0; i<SolidNodeList.size(); ++i)
			SolidNodeList[i].Node->render();

		Parameters->setAttribute("drawn_solid", (s32) SolidNodeList.size());		
	}

	// render shadow cast
	if (renderPass == ESNRP_SHADOW)
	{
		for (i=0; i<ShadowNodeList.size(); ++i)
			ShadowNodeList[i]->render();		
	}

	// render transparent objects.
	if (renderPass == ESNRP_TRANSPARENT)
	{
		CurrentRendertime = ESNRP_TRANSPARENT;

		for (i=0; i<TransparentNodeList.size(); ++i)
			TransparentNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent", (s32) TransparentNodeList.size());
	}

	// render transparent effect objects.
	if (renderPass == ESNRP_TRANSPARENT_EFFECT)
	{
		CurrentRendertime = ESNRP_TRANSPARENT_EFFECT;

		for (i=0; i<TransparentEffectNodeList.size(); ++i)
			TransparentEffectNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent_effect", (s32) TransparentEffectNodeList.size());
	}

	CurrentRendertime = ESNRP_NONE;
}

//! This method is called just before the rendering process of the whole scene.
//! draws all scene nodes
void CSceneManager::drawAll()
{
	if (!Driver)
		return;

	// reset attributes
	Parameters->setAttribute("culled", 0);
	Parameters->setAttribute("calls", 0);
	Parameters->setAttribute("drawn_solid", 0);
	Parameters->setAttribute("drawn_transparent", 0);
	Parameters->setAttribute("drawn_transparent_effect", 0);

	u32 i; // new ISO for scoping problem in some compilers

	// reset all transforms
	Driver->setMaterial(video::SMaterial());
	Driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );	
	Driver->setAllowZWriteOnTransparent(Parameters->getAttributeAsBool(ALLOW_ZWRITE_ON_TRANSPARENT));

	// do animations and other stuff.
	OnAnimate(os::Timer::getTime());

	/*!
		First Scene Node for prerendering should be the active camera
		consistent Camera is needed for culling
	*/
	camWorldPos.set(0,0,0);
	if (ActiveCamera)
	{
		ActiveCamera->render();
		camWorldPos = ActiveCamera->getAbsolutePosition();
	}

	// let all nodes register themselves
	OnRegisterSceneNode();

	//render camera scenes
	{
		CurrentRendertime = ESNRP_CAMERA;

		for (i=0; i<CameraList.size(); ++i)
			CameraList[i]->render();

		CameraList.set_used(0);

	}

	//render lights scenes
	{
		CurrentRendertime = ESNRP_LIGHT;

		// Sort the lights by distance from the camera
		core::vector3df camWorldPos(0, 0, 0);
		if (ActiveCamera)
			camWorldPos = ActiveCamera->getAbsolutePosition();

		core::array<DistanceNodeEntry> SortedLights;
		SortedLights.set_used(LightList.size());
		for (s32 light = (s32)LightList.size() - 1; light >= 0; --light)
			SortedLights[light].setNodeAndDistanceFromPosition(LightList[light], camWorldPos);

		SortedLights.set_sorted(false);
		SortedLights.sort();

		for(s32 light = (s32)LightList.size() - 1; light >= 0; --light)
			LightList[light] = SortedLights[light].Node;


		Driver->deleteAllDynamicLights();

		Driver->setAmbientLight(AmbientLight);

		u32 maxLights = LightList.size();

		for (i=0; i< maxLights; ++i)
			LightList[i]->render();
	}

	// render skyboxes
	{
		CurrentRendertime = ESNRP_SKY_BOX;

		for (i=0; i<SkyBoxList.size(); ++i)
			SkyBoxList[i]->render();

		SkyBoxList.set_used(0);
	}


	// render default objects
	{
		CurrentRendertime = ESNRP_SOLID;

		SolidNodeList.sort(); // sort by textures

		for (i=0; i<SolidNodeList.size(); ++i)
			SolidNodeList[i].Node->render();

		Parameters->setAttribute("drawn_solid", (s32) SolidNodeList.size());
		SolidNodeList.set_used(0);
	}

	// render transparent objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT;

		TransparentNodeList.sort(); // sort by distance from camera

		for (i=0; i<TransparentNodeList.size(); ++i)
			TransparentNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent", (s32) TransparentNodeList.size());
		TransparentNodeList.set_used(0);
	}

	// render transparent effect objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT_EFFECT;

		TransparentEffectNodeList.sort(); // sort by distance from camera

		for (i=0; i<TransparentEffectNodeList.size(); ++i)
			TransparentEffectNodeList[i].Node->render();

		Parameters->setAttribute("drawn_transparent_effect", (s32) TransparentEffectNodeList.size());
		TransparentEffectNodeList.set_used(0);
	}


	LightList.set_used(0);
	clearDeletionList();

	CurrentRendertime = ESNRP_NONE;
}


//! Sets the color of stencil buffers shadows drawn by the scene manager.
void CSceneManager::setShadowColor(video::SColor color)
{
	ShadowColor = color;
}


//! Returns the current color of shadows.
video::SColor CSceneManager::getShadowColor() const
{
	return ShadowColor;
}

//! Returns a pointer to the scene collision manager.
ISceneCollisionManager* CSceneManager::getSceneCollisionManager()
{
	return CollisionManager;
}

//! Returns a pointer to the mesh manipulator.
IMeshManipulator* CSceneManager::getMeshManipulator()
{
	return MeshManipulator;
}

//! Creates a simple ITriangleSelector, based on a mesh.
ITriangleSelector* CSceneManager::createTriangleSelector(IMesh* mesh, ISceneNode* node)
{
	if (!mesh)
		return 0;

	return new CTriangleSelector(mesh, node);
}

//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
ITriangleSelector* CSceneManager::createTriangleSelectorFromBoundingBox(ISceneNode* node)
{
	if (!node)
		return 0;

	return new CTriangleBBSelector(node);
}


//! Creates a simple ITriangleSelector, based on a mesh.
ITriangleSelector* CSceneManager::createOctreeTriangleSelector(IMesh* mesh,
							ISceneNode* node, s32 minimalPolysPerNode)
{
	if (!mesh)
		return 0;

	return new COctreeTriangleSelector(mesh, node, minimalPolysPerNode);
}


//! Creates a meta triangle selector.
IMetaTriangleSelector* CSceneManager::createMetaTriangleSelector()
{
	return new CMetaTriangleSelector();
}

//! Adds a scene node to the deletion queue.
void CSceneManager::addToDeletionQueue(ISceneNode* node)
{
	if (!node)
		return;

	node->grab();
	DeletionList.push_back(node);
}


//! clears the deletion list
void CSceneManager::clearDeletionList()
{
	if (DeletionList.empty())
		return;

	for (u32 i=0; i<DeletionList.size(); ++i)
	{
		DeletionList[i]->remove();
		DeletionList[i]->drop();
	}

	DeletionList.clear();
}


//! Returns the first scene node with the specified name.
ISceneNode* CSceneManager::getSceneNodeFromName(const char* name, ISceneNode* start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (!strcmp(start->getName(),name))
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromName(name, *it);
		if (node)
			return node;
	}

	return 0;
}


//! Returns the first scene node with the specified id.
ISceneNode* CSceneManager::getSceneNodeFromId(s32 id, ISceneNode* start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getID() == id)
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromId(id, *it);
		if (node)
			return node;
	}

	return 0;
}


//! Returns the first scene node with the specified type.
ISceneNode* CSceneManager::getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, ISceneNode* start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromType(type, *it);
		if (node)
			return node;
	}

	return 0;
}


//! returns scene nodes by type.
void CSceneManager::getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes, ISceneNode* start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		outNodes.push_back(start);

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();

	for (; it!=list.end(); ++it)
	{
		getSceneNodesFromType(type, outNodes, *it);
	}
}


//! Posts an input event to the environment. Usually you do not have to
//! use this method, it is used by the internal engine.
bool CSceneManager::postEventFromUser(const SEvent& event)
{
	bool ret = false;
	ICameraSceneNode* cam = getActiveCamera();
	if (cam)
		ret = cam->OnEvent(event);

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! Removes all children of this scene node
void CSceneManager::removeAll()
{
	ISceneNode::removeAll();
	setActiveCamera(0);
	// Make sure the driver is reset, might need a more complex method at some point
	if (Driver)
		Driver->setMaterial(video::SMaterial());
}


//! Clears the whole scene. All scene nodes are removed.
void CSceneManager::clear()
{
	removeAll();
}


//! Returns interface to the parameters set in this scene.
io::IAttributes* CSceneManager::getParameters()
{
	return Parameters;
}


//! Returns current render pass.
E_SCENE_NODE_RENDER_PASS CSceneManager::getSceneNodeRenderPass() const
{
	return CurrentRendertime;
}

//! Creates a new scene manager.
ISceneManager* CSceneManager::createNewSceneManager(bool cloneContent)
{
	CSceneManager* manager = new CSceneManager(Driver, FileSystem, CursorControl, MeshCache, GUIEnvironment);

	if (cloneContent)
		manager->cloneMembers(this, manager);

	return manager;
}

//! Writes attributes of the scene node.
void CSceneManager::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addString	("Name", Name.c_str());
	out->addInt	("Id", ID );
	out->addColorf	("AmbientLight", AmbientLight);

	// fog attributes from video driver
	video::SColor color;
	video::E_FOG_TYPE fogType;
	f32 start, end, density;
	bool pixelFog, rangeFog;

	Driver->getFog(color, fogType, start, end, density, pixelFog, rangeFog);

	out->addEnum("FogType", fogType, video::FogTypeNames);
	out->addColorf("FogColor", color);
	out->addFloat("FogStart", start);
	out->addFloat("FogEnd", end);
	out->addFloat("FogDensity", density);
	out->addBool("FogPixel", pixelFog);
	out->addBool("FogRange", rangeFog);
}

//! Reads attributes of the scene node.
void CSceneManager::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Name = in->getAttributeAsString("Name");
	ID = in->getAttributeAsInt("Id");
	AmbientLight = in->getAttributeAsColorf("AmbientLight");

	// fog attributes
	video::SColor color;
	video::E_FOG_TYPE fogType;
	f32 start, end, density;
	bool pixelFog, rangeFog;
	if (in->existsAttribute("FogType"))
	{
		fogType = (video::E_FOG_TYPE) in->getAttributeAsEnumeration("FogType", video::FogTypeNames);
		color = in->getAttributeAsColorf("FogColor").toSColor();
		start = in->getAttributeAsFloat("FogStart");
		end = in->getAttributeAsFloat("FogEnd");
		density = in->getAttributeAsFloat("FogDensity");
		pixelFog = in->getAttributeAsBool("FogPixel");
		rangeFog = in->getAttributeAsBool("FogRange");
		Driver->setFog(color, fogType, start, end, density, pixelFog, rangeFog);
	}

	RelativeTranslation.set(0,0,0);
	RelativeRotation.set(0,0,0);
	RelativeScale.set(1,1,1);
	IsVisible = true;
	AutomaticCullingState = scene::EAC_BOX;
	DebugDataVisible = scene::EDS_OFF;
	IsDebugObject = false;

	updateAbsolutePosition();
}


//! Sets ambient color of the scene
void CSceneManager::setAmbientLight(const video::SColorf &ambientColor)
{
	AmbientLight = ambientColor;
}


//! Returns ambient color of the scene
const video::SColorf& CSceneManager::getAmbientLight() const
{
	return AmbientLight;
}

// creates a scenemanager
ISceneManager* createSceneManager(video::IVideoDriver* driver,
		io::IFileSystem* fs, gui::ICursorControl* cursorcontrol,
		gui::IGUIEnvironment *guiEnvironment)
{
	return new CSceneManager(driver, fs, cursorcontrol, 0, guiEnvironment );
}


} // end namespace scene
} // end namespace irr

