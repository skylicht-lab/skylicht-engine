// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_MANAGER_H_INCLUDED__
#define __I_SCENE_MANAGER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "irrArray.h"
#include "irrString.h"
#include "path.h"
#include "vector3d.h"
#include "dimension2d.h"
#include "SColor.h"
#include "ESceneNodeTypes.h"
#include "ESceneNodeAnimatorTypes.h"
#include "SceneParameters.h"
#include "IGeometryCreator.h"
#include "IMeshManipulator.h"
#include "CMeshBuffer.h"

namespace irr
{
	struct SKeyMap;
	struct SEvent;

namespace io
{
	class IReadFile;
	class IAttributes;
	class IWriteFile;
	class IFileSystem;
} // end namespace io

namespace gui
{
	class IGUIFont;
	class IGUIEnvironment;
} // end namespace gui

namespace video
{
	class IVideoDriver;
	class SMaterial;
	class IImage;
	class ITexture;
} // end namespace video

namespace scene
{
	//! Enumeration for render passes.
	/** A parameter passed to the registerNodeForRendering() method of the ISceneManager,
	specifying when the node wants to be drawn in relation to the other nodes. */
	enum E_SCENE_NODE_RENDER_PASS
	{
		//! No pass currently active
		ESNRP_NONE = 0,

		//! Camera pass. The active view is set up here. The very first pass.
		ESNRP_CAMERA = 1,

		//! In this pass, lights are transformed into camera space and added to the driver
		ESNRP_LIGHT = 2,

		//! This is used for sky boxes.
		ESNRP_SKY_BOX = 4,

		//! All normal objects can use this for registering themselves.
		/** This value will never be returned by
		ISceneManager::getSceneNodeRenderPass(). The scene manager
		will determine by itself if an object is transparent or solid
		and register the object as SNRT_TRANSPARENT or SNRT_SOLD
		automatically if you call registerNodeForRendering with this
		value (which is default). Note that it will register the node
		only as ONE type. If your scene node has both solid and
		transparent material types register it twice (one time as
		SNRT_SOLID, the other time as SNRT_TRANSPARENT) and in the
		render() method call getSceneNodeRenderPass() to find out the
		current render pass and render only the corresponding parts of
		the node. */
		ESNRP_AUTOMATIC = 24,

		//! Solid scene nodes or special scene nodes without materials.
		ESNRP_SOLID = 8,
		ESNRP_SOLID_DEFERRED = 16,

		//! Transparent scene nodes, drawn after solid nodes. They are sorted from back to front and drawn in that order.
		ESNRP_TRANSPARENT = 32,

		//! Transparent effect scene nodes, drawn after Transparent nodes. They are sorted from back to front and drawn in that order.
		ESNRP_TRANSPARENT_EFFECT = 64,

		//! Drawn after the solid nodes, before the transparent nodes, the time for drawing shadow volumes
		ESNRP_SHADOW = 128,

		//! Skylicht engine cube reflect
		ESNRP_CUBE_REFLECT = 512
	};
	
	class ICameraSceneNode;
	class ILightSceneNode;
	class IMesh;
	class IMeshBuffer;	
	class IMeshSceneNode;
	class IMetaTriangleSelector;
	class IParticleSystemSceneNode;
	class ISceneCollisionManager;
	class ISceneNode;
	class ISceneNodeAnimator;
	class ISceneNodeAnimatorCollisionResponse;
	class ISceneNodeAnimatorFactory;
	class ISceneNodeFactory;
	class ISceneUserDataSerializer;	

	class ITriangleSelector;

	//! The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
	/** All Scene nodes can be created only here. There is a always growing
	list of scene nodes for lots of purposes: Indoor rendering scene nodes
	like the Octree (addOctreeSceneNode()) or the terrain renderer
	(addTerrainSceneNode()), different Camera scene nodes
	(addCameraSceneNode(), addCameraSceneNodeMaya()), scene nodes for Light
	(addLightSceneNode()), Billboards (addBillboardSceneNode()) and so on.
	A scene node is a node in the hierachical scene graph. Every scene node
	may have children, which are other scene nodes. Children move relative
	the their parents position. If the parent of a node is not visible, its
	children won't be visible, too. In this way, it is for example easily
	possible to attach a light to a moving car or to place a walking
	character on a moving platform on a moving ship.
	The SceneManager is also able to load 3d mesh files of different
	formats. Take a look at getMesh() to find out what formats are
	supported. If these formats are not enough, use
	addExternalMeshLoader() to add new formats to the engine.
	*/
	class ISceneManager : public virtual IReferenceCounted
	{
	public:

		//! Get the video driver.
		/** \return Pointer to the video Driver.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual video::IVideoDriver* getVideoDriver() = 0;

		//! Get the active GUIEnvironment
		/** \return Pointer to the GUIEnvironment
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual gui::IGUIEnvironment* getGUIEnvironment() = 0;

		//! Get the active FileSystem
		/** \return Pointer to the FileSystem
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual io::IFileSystem* getFileSystem() = 0;		

		//! Adds a camera scene node to the scene graph and sets it as active camera.
		/** This camera does not react on user input like for example the one created with
		addCameraSceneNodeFPS(). If you want to move or animate it, use animators or the
		ISceneNode::setPosition(), ICameraSceneNode::setTarget() etc methods.
		By default, a camera's look at position (set with setTarget()) and its scene node
		rotation (set with setRotation()) are independent. If you want to be able to
		control the direction that the camera looks by using setRotation() then call
		ICameraSceneNode::bindTargetAndRotation(true) on it.
		\param position: Position of the space relative to its parent where the camera will be placed.
		\param lookat: Position where the camera will look at. Also known as target.
		\param parent: Parent scene node of the camera. Can be null. If the parent moves,
		the camera will move too.
		\param id: id of the camera. This id can be used to identify the camera.
		\param makeActive Flag whether this camera should become the active one.
		Make sure you always have one active camera.
		\return Pointer to interface to camera if successful, otherwise 0.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& lookat = core::vector3df(0,0,100),
			s32 id=-1, bool makeActive=true) = 0;

		//! Adds a maya style user controlled camera scene node to the scene graph.
		/** This is a standard camera with an animator that provides mouse control similar
		to camera in the 3D Software Maya by Alias Wavefront.
		The camera does not react on setPosition anymore after applying this animator. Instead
		use setTarget, to fix the target the camera the camera hovers around. And setDistance
		to set the current distance from that target, i.e. the radius of the orbit the camera
		hovers on.
		\param parent: Parent scene node of the camera. Can be null.
		\param rotateSpeed: Rotation speed of the camera.
		\param zoomSpeed: Zoom speed of the camera.
		\param translationSpeed: TranslationSpeed of the camera.
		\param id: id of the camera. This id can be used to identify the camera.
		\param distance Initial distance of the camera from the object
		\param makeActive Flag whether this camera should become the active one.
		Make sure you always have one active camera.
		\return Returns a pointer to the interface of the camera if successful, otherwise 0.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent=0,
			f32 rotateSpeed=-1500.f, f32 zoomSpeed=200.f,
			f32 translationSpeed=1500.f, s32 id=-1, f32 distance=70.f,
			bool makeActive=true) =0;

		//! Adds a camera scene node with an animator which provides mouse and keyboard control appropriate for first person shooters (FPS).
		/** This FPS camera is intended to provide a demonstration of a
		camera that behaves like a typical First Person Shooter. It is
		useful for simple demos and prototyping but is not intended to
		provide a full solution for a production quality game. It binds
		the camera scene node rotation to the look-at target; @see
		ICameraSceneNode::bindTargetAndRotation(). With this camera,
		you look with the mouse, and move with cursor keys. If you want
		to change the key layout, you can specify your own keymap. For
		example to make the camera be controlled by the cursor keys AND
		the keys W,A,S, and D, do something like this:
		\code
		 SKeyMap keyMap[8];
		 keyMap[0].Action = EKA_MOVE_FORWARD;
		 keyMap[0].KeyCode = KEY_UP;
		 keyMap[1].Action = EKA_MOVE_FORWARD;
		 keyMap[1].KeyCode = KEY_KEY_W;

		 keyMap[2].Action = EKA_MOVE_BACKWARD;
		 keyMap[2].KeyCode = KEY_DOWN;
		 keyMap[3].Action = EKA_MOVE_BACKWARD;
		 keyMap[3].KeyCode = KEY_KEY_S;

		 keyMap[4].Action = EKA_STRAFE_LEFT;
		 keyMap[4].KeyCode = KEY_LEFT;
		 keyMap[5].Action = EKA_STRAFE_LEFT;
		 keyMap[5].KeyCode = KEY_KEY_A;

		 keyMap[6].Action = EKA_STRAFE_RIGHT;
		 keyMap[6].KeyCode = KEY_RIGHT;
		 keyMap[7].Action = EKA_STRAFE_RIGHT;
		 keyMap[7].KeyCode = KEY_KEY_D;

		camera = sceneManager->addCameraSceneNodeFPS(0, 100, 500, -1, keyMap, 8);
		\endcode
		\param parent: Parent scene node of the camera. Can be null.
		\param rotateSpeed: Speed in degress with which the camera is
		rotated. This can be done only with the mouse.
		\param moveSpeed: Speed in units per millisecond with which
		the camera is moved. Movement is done with the cursor keys.
		\param id: id of the camera. This id can be used to identify
		the camera.
		\param keyMapArray: Optional pointer to an array of a keymap,
		specifying what keys should be used to move the camera. If this
		is null, the default keymap is used. You can define actions
		more then one time in the array, to bind multiple keys to the
		same action.
		\param keyMapSize: Amount of items in the keymap array.
		\param noVerticalMovement: Setting this to true makes the
		camera only move within a horizontal plane, and disables
		vertical movement as known from most ego shooters. Default is
		'false', with which it is possible to fly around in space, if
		no gravity is there.
		\param jumpSpeed: Speed with which the camera is moved when
		jumping.
		\param invertMouse: Setting this to true makes the camera look
		up when the mouse is moved down and down when the mouse is
		moved up, the default is 'false' which means it will follow the
		movement of the mouse cursor.
		\param makeActive Flag whether this camera should become the active one.
		Make sure you always have one active camera.
		\return Pointer to the interface of the camera if successful,
		otherwise 0. This pointer should not be dropped. See
		IReferenceCounted::drop() for more information. */
		virtual ICameraSceneNode* addCameraSceneNodeFPS(ISceneNode* parent = 0,
			f32 rotateSpeed = 100.0f, f32 moveSpeed = 0.5f, s32 id=-1,
			SKeyMap* keyMapArray=0, s32 keyMapSize=0, bool noVerticalMovement=false,
			f32 jumpSpeed = 0.f, bool invertMouse=false,
			bool makeActive=true) = 0;

		//! Adds a dynamic light scene node to the scene graph.
		/** The light will cast dynamic light on all
		other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
		turned on. (This is the default setting in most scene nodes).
		\param parent: Parent scene node of the light. Can be null. If the parent moves,
		the light will move too.
		\param position: Position of the space relative to its parent where the light will be placed.
		\param color: Diffuse color of the light. Ambient or Specular colors can be set manually with
		the ILightSceneNode::getLightData() method.
		\param radius: Radius of the light.
		\param id: id of the node. This id can be used to identify the node.
		\return Pointer to the interface of the light if successful, otherwise NULL.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ILightSceneNode* addLightSceneNode(ISceneNode* parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),
			f32 radius=100.0f, s32 id=-1) = 0;

		//! Adds an empty scene node to the scene graph.
		/** Can be used for doing advanced transformations
		or structuring the scene graph.
		\return Pointer to the created scene node.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* addEmptySceneNode(ISceneNode* parent=0, s32 id=-1) = 0;

		//! Gets the root scene node.
		/** This is the scene node which is parent
		of all scene nodes. The root scene node is a special scene node which
		only exists to manage all scene nodes. It will not be rendered and cannot
		be removed from the scene.
		\return Pointer to the root scene node.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* getRootSceneNode() = 0;

		//! Get the first scene node with the specified id.
		/** \param id: The id to search for
		\param start: Scene node to start from. All children of this scene
		node are searched. If null is specified, the root scene node is
		taken.
		\return Pointer to the first scene node with this id,
		and null if no scene node could be found.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0) = 0;

		//! Get the first scene node with the specified name.
		/** \param name: The name to search for
		\param start: Scene node to start from. All children of this scene
		node are searched. If null is specified, the root scene node is
		taken.
		\return Pointer to the first scene node with this id,
		and null if no scene node could be found.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* getSceneNodeFromName(const c8* name, ISceneNode* start=0) = 0;

		//! Get the first scene node with the specified type.
		/** \param type: The type to search for
		\param start: Scene node to start from. All children of this scene
		node are searched. If null is specified, the root scene node is
		taken.
		\return Pointer to the first scene node with this type,
		and null if no scene node could be found.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, ISceneNode* start=0) = 0;

		//! Get scene nodes by type.
		/** \param type: Type of scene node to find (ESNT_ANY will return all child nodes).
		\param outNodes: array to be filled with results.
		\param start: Scene node to start from. All children of this scene
		node are searched. If null is specified, the root scene node is
		taken. */
		virtual void getSceneNodesFromType(ESCENE_NODE_TYPE type,
				core::array<scene::ISceneNode*>& outNodes,
				ISceneNode* start=0) = 0;

		//! Get the current active camera.
		/** \return The active camera is returned. Note that this can
		be NULL, if there was no camera created yet.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ICameraSceneNode* getActiveCamera() const =0;

		//! Sets the currently active camera.
		/** The previous active camera will be deactivated.
		\param camera: The new camera which should be active. */
		virtual void setActiveCamera(ICameraSceneNode* camera) = 0;

		//! Sets the color of stencil buffers shadows drawn by the scene manager.
		virtual void setShadowColor(video::SColor color = video::SColor(150,0,0,0)) = 0;

		//! Get the current color of shadows.
		virtual video::SColor getShadowColor() const = 0;

		//! Registers a node for rendering it at a specific time.
		/** This method should only be used by SceneNodes when they get a
		ISceneNode::OnRegisterSceneNode() call.
		\param node: Node to register for drawing. Usually scene nodes would set 'this'
		as parameter here because they want to be drawn.
		\param pass: Specifies when the node wants to be drawn in relation to the other nodes.
		For example, if the node is a shadow, it usually wants to be drawn after all other nodes
		and will use ESNRP_SHADOW for this. See scene::E_SCENE_NODE_RENDER_PASS for details.
		\return scene will be rendered ( passed culling ) */
		virtual u32 registerNodeForRendering(ISceneNode* node,
			E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC) = 0;

		//! Draws all the scene nodes.
		/** This can only be invoked between
		IVideoDriver::beginScene() and IVideoDriver::endScene(). Please note that
		the scene is not only drawn when calling this, but also animated
		by existing scene node animators, culling of scene nodes is done, etc. */
		virtual void drawAll() = 0;

		virtual void update() = 0;
		virtual void draw() = 0;
		virtual void draw(E_SCENE_NODE_RENDER_PASS renderPass) = 0;

		//! Creates a rotation animator, which rotates the attached scene node around itself.
		/** \param rotationSpeed Specifies the speed of the animation in degree per 10 milliseconds.
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createRotationAnimator(const core::vector3df& rotationSpeed) = 0;

		//! Creates a fly circle animator, which lets the attached scene node fly around a center.
		/** \param center: Center of the circle.
		\param radius: Radius of the circle.
		\param speed: The orbital speed, in radians per millisecond.
		\param direction: Specifies the upvector used for alignment of the mesh.
		\param startPosition: The position on the circle where the animator will
		begin. Value is in multiples of a circle, i.e. 0.5 is half way around. (phase)
		\param radiusEllipsoid: if radiusEllipsoid != 0 then radius2 froms a ellipsoid
		begin. Value is in multiples of a circle, i.e. 0.5 is half way around. (phase)
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createFlyCircleAnimator(
				const core::vector3df& center=core::vector3df(0.f,0.f,0.f),
				f32 radius=100.f, f32 speed=0.001f,
				const core::vector3df& direction=core::vector3df(0.f, 1.f, 0.f),
				f32 startPosition = 0.f,
				f32 radiusEllipsoid = 0.f) = 0;

		//! Creates a fly straight animator, which lets the attached scene node fly or move along a line between two points.
		/** \param startPoint: Start point of the line.
		\param endPoint: End point of the line.
		\param timeForWay: Time in milli seconds how long the node should need to
		move from the start point to the end point.
		\param loop: If set to false, the node stops when the end point is reached.
		If loop is true, the node begins again at the start.
		\param pingpong Flag to set whether the animator should fly
		back from end to start again.
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createFlyStraightAnimator(const core::vector3df& startPoint,
			const core::vector3df& endPoint, u32 timeForWay, bool loop=false, bool pingpong = false) = 0;

		//! Creates a texture animator, which switches the textures of the target scene node based on a list of textures.
		/** \param textures: List of textures to use.
		\param timePerFrame: Time in milliseconds, how long any texture in the list
		should be visible.
		\param loop: If set to to false, the last texture remains set, and the animation
		stops. If set to true, the animation restarts with the first texture.
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createTextureAnimator(const core::array<video::ITexture*>& textures,
			s32 timePerFrame, bool loop=true) = 0;

		//! Creates a scene node animator, which deletes the scene node after some time automatically.
		/** \param timeMs: Time in milliseconds, after when the node will be deleted.
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createDeleteAnimator(u32 timeMs) = 0;

		//! Creates a special scene node animator for doing automatic collision detection and response.
		/** See ISceneNodeAnimatorCollisionResponse for details.
		\param world: Triangle selector holding all triangles of the world with which
		the scene node may collide. You can create a triangle selector with
		ISceneManager::createTriangleSelector();
		\param sceneNode: SceneNode which should be manipulated. After you added this animator
		to the scene node, the scene node will not be able to move through walls and is
		affected by gravity. If you need to teleport the scene node to a new position without
		it being effected by the collision geometry, then call sceneNode->setPosition(); then
		animator->setTargetNode(sceneNode);
		\param ellipsoidRadius: Radius of the ellipsoid with which collision detection and
		response is done. If you have got a scene node, and you are unsure about
		how big the radius should be, you could use the following code to determine
		it:
		\code
		const core::aabbox3d<f32>& box = yourSceneNode->getBoundingBox();
		core::vector3df radius = box.MaxEdge - box.getCenter();
		\endcode
		\param gravityPerSecond: Sets the gravity of the environment, as an acceleration in
		units per second per second. If your units are equivalent to metres, then
		core::vector3df(0,-10.0f,0) would give an approximately realistic gravity.
		You can disable gravity by setting it to core::vector3df(0,0,0).
		\param ellipsoidTranslation: By default, the ellipsoid for collision detection is created around
		the center of the scene node, which means that the ellipsoid surrounds
		it completely. If this is not what you want, you may specify a translation
		for the ellipsoid.
		\param slidingValue: DOCUMENTATION NEEDED.
		\return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will cause it to do collision detection and response.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimatorCollisionResponse* createCollisionResponseAnimator(
			ITriangleSelector* world, ISceneNode* sceneNode,
			const core::vector3df& ellipsoidRadius = core::vector3df(30,60,30),
			const core::vector3df& gravityPerSecond = core::vector3df(0,-10.0f,0),
			const core::vector3df& ellipsoidTranslation = core::vector3df(0,0,0),
			f32 slidingValue = 0.0005f) = 0;

		//! Creates a follow spline animator.
		/** The animator modifies the position of
		the attached scene node to make it follow a hermite spline.
		It uses a subset of hermite splines: either cardinal splines
		(tightness != 0.5) or catmull-rom-splines (tightness == 0.5).
		The animator moves from one control point to the next in
		1/speed seconds. This code was sent in by Matthias Gall.
		If you no longer need the animator, you should call ISceneNodeAnimator::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimator* createFollowSplineAnimator(s32 startTime,
			const core::array< core::vector3df >& points,
			f32 speed = 1.0f, f32 tightness = 0.5f, bool loop=true, bool pingpong=false) = 0;

		//! Creates a simple ITriangleSelector, based on a mesh.
		/** Triangle selectors
		can be used for doing collision detection. Don't use this selector
		for a huge amount of triangles like in Quake3 maps.
		Instead, use for example ISceneManager::createOctreeTriangleSelector().
		Please note that the created triangle selector is not automaticly attached
		to the scene node. You will have to call ISceneNode::setTriangleSelector()
		for this. To create and attach a triangle selector is done like this:
		\code
		ITriangleSelector* s = sceneManager->createTriangleSelector(yourMesh,
				yourSceneNode);
		yourSceneNode->setTriangleSelector(s);
		s->drop();
		\endcode
		\param mesh: Mesh of which the triangles are taken.
		\param node: Scene node of which visibility and transformation is used.
		\return The selector, or null if not successful.
		If you no longer need the selector, you should call ITriangleSelector::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ITriangleSelector* createTriangleSelector(IMesh* mesh, ISceneNode* node) = 0;


		//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
		/** Triangle selectors
		can be used for doing collision detection. Every time when triangles are
		queried, the triangle selector gets the bounding box of the scene node,
		an creates new triangles. In this way, it works good with animated scene nodes.
		\param node: Scene node of which the bounding box, visibility and transformation is used.
		\return The selector, or null if not successful.
		If you no longer need the selector, you should call ITriangleSelector::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ITriangleSelector* createTriangleSelectorFromBoundingBox(ISceneNode* node) = 0;

		//! Creates a Triangle Selector, optimized by an octree.
		/** Triangle selectors
		can be used for doing collision detection. This triangle selector is
		optimized for huge amounts of triangle, it organizes them in an octree.
		Please note that the created triangle selector is not automaticly attached
		to the scene node. You will have to call ISceneNode::setTriangleSelector()
		for this. To create and attach a triangle selector is done like this:
		\code
		ITriangleSelector* s = sceneManager->createOctreeTriangleSelector(yourMesh,
				yourSceneNode);
		yourSceneNode->setTriangleSelector(s);
		s->drop();
		\endcode
		For more informations and examples on this, take a look at the collision
		tutorial in the SDK.
		\param mesh: Mesh of which the triangles are taken.
		\param node: Scene node of which visibility and transformation is used.
		\param minimalPolysPerNode: Specifies the minimal polygons contained a octree node.
		If a node gets less polys the this value, it will not be splitted into
		smaller nodes.
		\return The selector, or null if not successful.
		If you no longer need the selector, you should call ITriangleSelector::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ITriangleSelector* createOctreeTriangleSelector(IMesh* mesh,
			ISceneNode* node, s32 minimalPolysPerNode=32) = 0;

		//! //! Creates a Triangle Selector, optimized by an octree.
		/** \deprecated Use createOctreeTriangleSelector instead. This method may be removed by Irrlicht 1.9. */
		_IRR_DEPRECATED_ ITriangleSelector* createOctTreeTriangleSelector(IMesh* mesh,
			ISceneNode* node, s32 minimalPolysPerNode=32)
		{
			return createOctreeTriangleSelector(mesh, node, minimalPolysPerNode);
		}

		//! Creates a meta triangle selector.
		/** A meta triangle selector is nothing more than a
		collection of one or more triangle selectors providing together
		the interface of one triangle selector. In this way,
		collision tests can be done with different triangle soups in one pass.
		\return The selector, or null if not successful.
		If you no longer need the selector, you should call ITriangleSelector::drop().
		See IReferenceCounted::drop() for more information. */
		virtual IMetaTriangleSelector* createMetaTriangleSelector() = 0;

		//! Get pointer to the scene collision manager.
		/** \return Pointer to the collision manager
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneCollisionManager* getSceneCollisionManager() = 0;

		//! Returns a pointer to the mesh manipulator.
		virtual IMeshManipulator* getMeshManipulator() = 0;

		//! Adds a scene node to the deletion queue.
		/** The scene node is immediatly
		deleted when it's secure. Which means when the scene node does not
		execute animators and things like that. This method is for example
		used for deleting scene nodes by their scene node animators. In
		most other cases, a ISceneNode::remove() call is enough, using this
		deletion queue is not necessary.
		See ISceneManager::createDeleteAnimator() for details.
		\param node: Node to detete. */
		virtual void addToDeletionQueue(ISceneNode* node) = 0;

		//! Posts an input event to the environment.
		/** Usually you do not have to
		use this method, it is used by the internal engine. */
		virtual bool postEventFromUser(const SEvent& event) = 0;

		//! Clears the whole scene.
		/** All scene nodes are removed. */
		virtual void clear() = 0;

		//! Get interface to the parameters set in this scene.
		/** String parameters can be used by plugins and mesh loaders.
		For example the CMS and LMTS loader want a parameter named 'CSM_TexturePath'
		and 'LMTS_TexturePath' set to the path were attached textures can be found. See
		CSM_TEXTURE_PATH, LMTS_TEXTURE_PATH, MY3D_TEXTURE_PATH,
		COLLADA_CREATE_SCENE_INSTANCES, DMF_TEXTURE_PATH and DMF_USE_MATERIALS_DIRS*/
		virtual io::IAttributes* getParameters() = 0;

		//! Get current render pass.
		/** All scene nodes are being rendered in a specific order.
		First lights, cameras, sky boxes, solid geometry, and then transparent
		stuff. During the rendering process, scene nodes may want to know what the scene
		manager is rendering currently, because for example they registered for rendering
		twice, once for transparent geometry and once for solid. When knowing what rendering
		pass currently is active they can render the correct part of their geometry. */
		virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass() const = 0;

		//! Get the default scene node factory which can create all built in scene nodes
		/** \return Pointer to the default scene node factory
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeFactory* getDefaultSceneNodeFactory() = 0;

		//! Adds a scene node factory to the scene manager.
		/** Use this to extend the scene manager with new scene node types which it should be
		able to create automaticly, for example when loading data from xml files. */
		virtual void registerSceneNodeFactory(ISceneNodeFactory* factoryToAdd) = 0;

		//! Get amount of registered scene node factories.
		virtual u32 getRegisteredSceneNodeFactoryCount() const = 0;

		//! Get a scene node factory by index
		/** \return Pointer to the requested scene node factory, or 0 if it does not exist.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeFactory* getSceneNodeFactory(u32 index) = 0;

		//! Get the default scene node animator factory which can create all built-in scene node animators
		/** \return Pointer to the default scene node animator factory
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimatorFactory* getDefaultSceneNodeAnimatorFactory() = 0;

		//! Adds a scene node animator factory to the scene manager.
		/** Use this to extend the scene manager with new scene node animator types which it should be
		able to create automaticly, for example when loading data from xml files. */
		virtual void registerSceneNodeAnimatorFactory(ISceneNodeAnimatorFactory* factoryToAdd) = 0;

		//! Get amount of registered scene node animator factories.
		virtual u32 getRegisteredSceneNodeAnimatorFactoryCount() const = 0;

		//! Get scene node animator factory by index
		/** \return Pointer to the requested scene node animator factory, or 0 if it does not exist.
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNodeAnimatorFactory* getSceneNodeAnimatorFactory(u32 index) = 0;

		//! Get typename from a scene node type or null if not found
		virtual const c8* getSceneNodeTypeName(ESCENE_NODE_TYPE type) = 0;

		//! Returns a typename from a scene node animator type or null if not found
		virtual const c8* getAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type) = 0;

		//! Adds a scene node to the scene by name
		/** \return Pointer to the scene node added by a factory
		This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
		virtual ISceneNode* addSceneNode(const char* sceneNodeTypeName, ISceneNode* parent=0) = 0;

		//! creates a scene node animator based on its type name
		/** \param typeName: Type of the scene node animator to add.
		\param target: Target scene node of the new animator.
		\return Returns pointer to the new scene node animator or null if not successful. You need to
		drop this pointer after calling this, see IReferenceCounted::drop() for details. */
		virtual ISceneNodeAnimator* createSceneNodeAnimator(const char* typeName, ISceneNode* target=0) = 0;

		//! Creates a new scene manager.
		/** This can be used to easily draw and/or store two
		independent scenes at the same time. The mesh cache will be
		shared between all existing scene managers, which means if you
		load a mesh in the original scene manager using for example
		getMesh(), the mesh will be available in all other scene
		managers too, without loading.
		The original/main scene manager will still be there and
		accessible via IrrlichtDevice::getSceneManager(). If you need
		input event in this new scene manager, for example for FPS
		cameras, you'll need to forward input to this manually: Just
		implement an IEventReceiver and call
		yourNewSceneManager->postEventFromUser(), and return true so
		that the original scene manager doesn't get the event.
		Otherwise, all input will go to the main scene manager
		automatically.
		If you no longer need the new scene manager, you should call
		ISceneManager::drop().
		See IReferenceCounted::drop() for more information. */
		virtual ISceneManager* createNewSceneManager(bool cloneContent=false) = 0;	

		//! Sets ambient color of the scene
		virtual void setAmbientLight(const video::SColorf &ambientColor) = 0;

		//! Get ambient color of the scene
		virtual const video::SColorf& getAmbientLight() const = 0;

		//! Get an instance of a geometry creator.
		/** The geometry creator provides some helper methods to create various types of
		basic geometry. This can be useful for custom scene nodes. */
		virtual const IGeometryCreator* getGeometryCreator(void) const = 0;

		//! Check if node is culled in current view frustum
		/** Please note that depending on the used culling method this
		check can be rather coarse, or slow. A positive result is
		correct, though, i.e. if this method returns true the node is
		positively not visible. The node might still be invisible even
		if this method returns false.
		\param node The scene node which is checked for culling.
		\return True if node is not visible in the current scene, else
		false. */
		virtual bool isCulled(const ISceneNode* node) const =0;

	};


} // end namespace scene
} // end namespace irr

#endif
