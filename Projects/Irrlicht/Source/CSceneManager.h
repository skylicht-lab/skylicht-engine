// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SCENE_MANAGER_H_INCLUDED__
#define __C_SCENE_MANAGER_H_INCLUDED__

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ICursorControl.h"
#include "irrString.h"
#include "irrArray.h"
#include "CAttributes.h"

namespace irr
{
namespace io
{
	class IXMLWriter;
	class IFileSystem;
}
namespace scene
{
	class IMeshCache;
	class IGeometryCreator;

	/*!
		The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
	*/
	class CSceneManager : public ISceneManager, public ISceneNode
	{
	public:

		//! constructor
		CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs,
			gui::ICursorControl* cursorControl, IMeshCache* cache = 0,
			gui::IGUIEnvironment *guiEnvironment = 0);

		//! destructor
		virtual ~CSceneManager();

		//! returns the video driver
		virtual video::IVideoDriver* getVideoDriver();

		//! return the gui environment
		virtual gui::IGUIEnvironment* getGUIEnvironment();

		//! return the filesystem
		virtual io::IFileSystem* getFileSystem();	

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! registers a node for rendering it at a specific time.
		virtual u32 registerNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC);

		//! draws all scene nodes
		virtual void drawAll();

		virtual void update();
		virtual void draw();
		virtual void draw(E_SCENE_NODE_RENDER_PASS renderPass);

		//! Adds a camera scene node to the tree and sets it as active camera.
		//! \param position: Position of the space relative to its parent where the camera will be placed.
		//! \param lookat: Position where the camera will look at. Also known as target.
		//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
		//! the camera will move too.
		//! \return Pointer to interface to camera
		virtual ICameraSceneNode* addCameraSceneNode(ISceneNode* parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& lookat = core::vector3df(0,0,100),
			s32 id=-1, bool makeActive=true);

		//! Adds a camera scene node which is able to be controlle with the mouse similar
		//! like in the 3D Software Maya by Alias Wavefront.
		//! The returned pointer must not be dropped.
		virtual ICameraSceneNode* addCameraSceneNodeMaya(ISceneNode* parent=0,
			f32 rotateSpeed=-1500.f, f32 zoomSpeed=200.f,
			f32 translationSpeed=1500.f, s32 id=-1, f32 distance=70.f,
			bool makeActive=true);

		//! Adds a camera scene node which is able to be controled with the mouse and keys
		//! like in most first person shooters (FPS):
		virtual ICameraSceneNode* addCameraSceneNodeFPS(ISceneNode* parent = 0,
			f32 rotateSpeed = 100.0f, f32 moveSpeed = .5f, s32 id=-1,
			SKeyMap* keyMapArray=0, s32 keyMapSize=0,
			bool noVerticalMovement=false, f32 jumpSpeed = 0.f,
			bool invertMouseY=false, bool makeActive=true);

		//! Adds a dynamic light scene node. The light will cast dynamic light on all
		//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
		//! turned on. (This is the default setting in most scene nodes).
		virtual ILightSceneNode* addLightSceneNode(ISceneNode* parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),
			f32 range=100.0f, s32 id=-1);

		//! Adds an empty scene node.
		virtual ISceneNode* addEmptySceneNode(ISceneNode* parent, s32 id=-1);

		//! Returns the root scene node. This is the scene node wich is parent
		//! of all scene nodes. The root scene node is a special scene node which
		//! only exists to manage all scene nodes. It is not rendered and cannot
		//! be removed from the scene.
		//! \return Pointer to the root scene node.
		virtual ISceneNode* getRootSceneNode();

		//! Returns the current active camera.
		//! \return The active camera is returned. Note that this can be NULL, if there
		//! was no camera created yet.
		virtual ICameraSceneNode* getActiveCamera() const;

		//! Sets the active camera. The previous active camera will be deactivated.
		//! \param camera: The new camera which should be active.
		virtual void setActiveCamera(ICameraSceneNode* camera);

		//! creates a rotation animator, which rotates the attached scene node around itself.
		//! \param rotationPerSecond: Specifies the speed of the animation
		//! \return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		//! and the animator will animate it.
		virtual ISceneNodeAnimator* createRotationAnimator(const core::vector3df& rotationPerSecond);

		//! creates a fly circle animator
		/** Lets the attached scene node fly around a center.
		\param center Center relative to node origin
		 \param speed: The orbital speed, in radians per millisecond.
		 \param direction: Specifies the upvector used for alignment of the mesh.
		 \param startPosition: The position on the circle where the animator will
			begin. Value is in multiples  of a circle, i.e. 0.5 is half way around.
		 \return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		 */
		virtual ISceneNodeAnimator* createFlyCircleAnimator(
				const core::vector3df& center=core::vector3df(0.f, 0.f, 0.f),
				f32 radius=100.f, f32 speed=0.001f,
				const core::vector3df& direction=core::vector3df(0.f, 1.f, 0.f),
				f32 startPosition = 0.f,
				f32 radiusEllipsoid = 0.f);

		//! Creates a fly straight animator, which lets the attached scene node
		//! fly or move along a line between two points.
		virtual ISceneNodeAnimator* createFlyStraightAnimator(const core::vector3df& startPoint,
			const core::vector3df& endPoint, u32 timeForWay, bool loop=false,bool pingpong = false);

		//! Creates a texture animator, which switches the textures of the target scene
		//! node based on a list of textures.
		virtual ISceneNodeAnimator* createTextureAnimator(const core::array<video::ITexture*>& textures,
			s32 timePerFrame, bool loop);

		//! Creates a scene node animator, which deletes the scene node after
		//! some time automaticly.
		virtual ISceneNodeAnimator* createDeleteAnimator(u32 timeMS);


		//! Creates a special scene node animator for doing automatic collision detection
		//! and response.
		virtual ISceneNodeAnimatorCollisionResponse* createCollisionResponseAnimator(
			ITriangleSelector* world, ISceneNode* sceneNode,
			const core::vector3df& ellipsoidRadius = core::vector3df(30,60,30),
			const core::vector3df& gravityPerSecond = core::vector3df(0,-1.0f,0),
			const core::vector3df& ellipsoidTranslation = core::vector3df(0,0,0),
			f32 slidingValue = 0.0005f);

		//! Creates a follow spline animator.
		virtual ISceneNodeAnimator* createFollowSplineAnimator(s32 startTime,
			const core::array< core::vector3df >& points,
			f32 speed, f32 tightness, bool loop, bool pingpong);


		//! Creates a simple ITriangleSelector, based on a mesh.
		virtual ITriangleSelector* createTriangleSelector(IMesh* mesh, ISceneNode* node);	

		//! Creates a simple ITriangleSelector, based on a mesh.
		virtual ITriangleSelector* createOctreeTriangleSelector(IMesh* mesh,
			ISceneNode* node, s32 minimalPolysPerNode);

		//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
		virtual ITriangleSelector* createTriangleSelectorFromBoundingBox(
			ISceneNode* node);

		//! Creates a meta triangle selector.
		virtual IMetaTriangleSelector* createMetaTriangleSelector();

		//! Returns a pointer to the scene collision manager.
		virtual ISceneCollisionManager* getSceneCollisionManager();

		//! Returns a pointer to the mesh manipulator.
		virtual IMeshManipulator* getMeshManipulator();

		//! Sets the color of stencil buffers shadows drawn by the scene manager.
		virtual void setShadowColor(video::SColor color);

		//! Returns the current color of shadows.
		virtual video::SColor getShadowColor() const;

		//! Adds a scene node to the deletion queue.
		virtual void addToDeletionQueue(ISceneNode* node);

		//! Returns the first scene node with the specified id.
		virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0);

		//! Returns the first scene node with the specified name.
		virtual ISceneNode* getSceneNodeFromName(const c8* name, ISceneNode* start=0);

		//! Returns the first scene node with the specified type.
		virtual ISceneNode* getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, ISceneNode* start=0);

		//! returns scene nodes by type.
		virtual void getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes, ISceneNode* start=0);

		//! Posts an input event to the environment. Usually you do not have to
		//! use this method, it is used by the internal engine.
		virtual bool postEventFromUser(const SEvent& event);

		//! Clears the whole scene. All scene nodes are removed.
		virtual void clear();

		//! Removes all children of this scene node
		virtual void removeAll();

		//! Returns interface to the parameters set in this scene.
		virtual io::IAttributes* getParameters();

		//! Returns current render pass.
		virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass() const;

		//! Creates a new scene manager.
		virtual ISceneManager* createNewSceneManager(bool cloneContent);

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_SCENE_MANAGER; }

		//! Returns the default scene node factory which can create all built in scene nodes
		virtual ISceneNodeFactory* getDefaultSceneNodeFactory();

		//! Adds a scene node factory to the scene manager.
		/** Use this to extend the scene manager with new scene node types which it should be
		able to create automaticly, for example when loading data from xml files. */
		virtual void registerSceneNodeFactory(ISceneNodeFactory* factoryToAdd);

		//! Returns amount of registered scene node factories.
		virtual u32 getRegisteredSceneNodeFactoryCount() const;

		//! Returns a scene node factory by index
		virtual ISceneNodeFactory* getSceneNodeFactory(u32 index);

		//! Returns a typename from a scene node type or null if not found
		virtual const c8* getSceneNodeTypeName(ESCENE_NODE_TYPE type);

		//! Returns a typename from a scene node animator type or null if not found
		virtual const c8* getAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type);

		//! Adds a scene node to the scene by name
		virtual ISceneNode* addSceneNode(const char* sceneNodeTypeName, ISceneNode* parent=0);

		//! creates a scene node animator based on its type name
		virtual ISceneNodeAnimator* createSceneNodeAnimator(const char* typeName, ISceneNode* target=0);

		//! Returns the default scene node animator factory which can create all built-in scene node animators
		virtual ISceneNodeAnimatorFactory* getDefaultSceneNodeAnimatorFactory();

		//! Adds a scene node animator factory to the scene manager.
		virtual void registerSceneNodeAnimatorFactory(ISceneNodeAnimatorFactory* factoryToAdd);

		//! Returns amount of registered scene node animator factories.
		virtual u32 getRegisteredSceneNodeAnimatorFactoryCount() const;

		//! Returns a scene node animator factory by index
		virtual ISceneNodeAnimatorFactory* getSceneNodeAnimatorFactory(u32 index);		

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		//! Sets ambient color of the scene
		virtual void setAmbientLight(const video::SColorf &ambientColor);

		//! Returns ambient color of the scene
		virtual const video::SColorf& getAmbientLight() const;

		//! Get current render time.
		virtual E_SCENE_NODE_RENDER_PASS getCurrentRendertime() const { return CurrentRendertime; }

		//! Set current render time.
		virtual void setCurrentRendertime(E_SCENE_NODE_RENDER_PASS currentRendertime) { CurrentRendertime = currentRendertime; }

		//! Get an instance of a geometry creator.
		virtual const IGeometryCreator* getGeometryCreator(void) const { return GeometryCreator; }

		//! returns if node is culled
		virtual bool isCulled(const ISceneNode* node) const;

	protected:

	private:

		//! clears the deletion list
		void clearDeletionList();

		//! writes a scene node
		void writeSceneNode(io::IXMLWriter* writer, ISceneNode* node, ISceneUserDataSerializer* userDataSerializer, const fschar_t* currentPath=0, bool init=false);

		struct DefaultNodeEntry
		{
			DefaultNodeEntry(ISceneNode* n) :
				Node(n), TextureValue(0)
			{
				if (n->getMaterialCount())
					TextureValue = (n->getMaterial(0).getTexture(0));
			}

			bool operator < (const DefaultNodeEntry& other) const
			{
				return (TextureValue < other.TextureValue);
			}

			ISceneNode* Node;
			private:
			void* TextureValue;
		};

		//! sort on distance (center) to camera
		struct TransparentNodeEntry
		{
			TransparentNodeEntry(ISceneNode* n, const core::vector3df& camera)
				: Node(n)
			{
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(camera);
			}

			bool operator < (const TransparentNodeEntry& other) const
			{
				return Distance > other.Distance;
			}

			ISceneNode* Node;
			private:
				f64 Distance;
		};

		//! sort on distance (sphere) to camera
		struct DistanceNodeEntry
		{
			DistanceNodeEntry(ISceneNode* n, const core::vector3df& cameraPos)
				: Node(n)
			{
				setNodeAndDistanceFromPosition(n, cameraPos);
			}

			bool operator < (const DistanceNodeEntry& other) const
			{
				return Distance < other.Distance;
			}

			void setNodeAndDistanceFromPosition(ISceneNode* n, const core::vector3df & fromPosition)
			{
				Node = n;
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(fromPosition);
				Distance -= Node->getBoundingBox().getExtent().getLengthSQ() * 0.5;
			}

			ISceneNode* Node;
			private:
			f64 Distance;
		};

		//! video driver
		video::IVideoDriver* Driver;

		//! file system
		io::IFileSystem* FileSystem;

		//! GUI Enviroment ( Debug Purpose )
		gui::IGUIEnvironment* GUIEnvironment;

		//! cursor control
		gui::ICursorControl* CursorControl;

		//! collision manager
		ISceneCollisionManager* CollisionManager;

		//! Mesh Manipulator
		IMeshManipulator* MeshManipulator;

		//! render pass lists
		core::array<ISceneNode*> CameraList;
		core::array<ISceneNode*> LightList;
		core::array<ISceneNode*> ShadowNodeList;
		core::array<DefaultNodeEntry> CubeReflectList;
		core::array<ISceneNode*> SkyBoxList;
		core::array<DefaultNodeEntry> SolidNodeList;
		core::array<DefaultNodeEntry> SolidDeferredNodeList;
		core::array<TransparentNodeEntry> TransparentNodeList;
		core::array<TransparentNodeEntry> TransparentEffectNodeList;
		
		core::array<ISceneNode*> DeletionList;
		core::array<ISceneNodeFactory*> SceneNodeFactoryList;
		core::array<ISceneNodeAnimatorFactory*> SceneNodeAnimatorFactoryList;

		//! current active camera
		ICameraSceneNode* ActiveCamera;
		core::vector3df camWorldPos; // Position of camera for transparent nodes.

		video::SColor ShadowColor;
		video::SColorf AmbientLight;

		//! String parameters
		io::CAttributes* Parameters;

		//! Mesh cache
		IMeshCache* MeshCache;

		E_SCENE_NODE_RENDER_PASS CurrentRendertime;

		//! constants for reading and writing XML.
		//! Not made static due to portability problems.
		const core::stringw IRR_XML_FORMAT_SCENE;
		const core::stringw IRR_XML_FORMAT_NODE;
		const core::stringw IRR_XML_FORMAT_NODE_ATTR_TYPE;

		IGeometryCreator* GeometryCreator;
	};

} // end namespace video
} // end namespace scene

#endif

