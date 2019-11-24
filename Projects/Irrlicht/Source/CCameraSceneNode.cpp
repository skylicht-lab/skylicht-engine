// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CCameraSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "irrOS.h"

namespace irr
{
namespace scene
{


//! constructor
CCameraSceneNode::CCameraSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
	const core::vector3df& position, const core::vector3df& lookat)
	: ICameraSceneNode(parent, mgr, id, position),
	Target(lookat), UpVector(0.0f, 1.0f, 0.0f), ZNear(1.0f), ZFar(3000.0f),
	InputReceiverEnabled(true), TargetAndRotationAreBound(false)
{
	#ifdef _DEBUG
	setDebugName("CCameraSceneNode");
	#endif

	// set default projection
	Fovy = core::PI / 2.5f;	// Field of view, in radians.
	
	LastTime = 0;
	ShakeDis = 0.0f;
	ShakeRandom = false;
	ShakeX = 0.0f;
	ShakeY = 0.0f;
	ShakeZ = 0.0f;
	ShakeD = 0.0f;

	const video::IVideoDriver* const d = mgr?mgr->getVideoDriver():0;
	if (d)
		Aspect = (f32)d->getCurrentRenderTargetSize().Width /
			(f32)d->getCurrentRenderTargetSize().Height;
	else
		Aspect = 4.0f / 3.0f;	// Aspect ratio.

	recalculateProjectionMatrix();
	recalculateViewArea();
	ViewArea.setFarNearDistance(ZFar - ZNear);
}


//! Disables or enables the camera to get key or mouse inputs.
void CCameraSceneNode::setInputReceiverEnabled(bool enabled)
{
	InputReceiverEnabled = enabled;
}


//! Returns if the input receiver of the camera is currently enabled.
bool CCameraSceneNode::isInputReceiverEnabled() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return InputReceiverEnabled;
}


//! Sets the projection matrix of the camera.
/** The core::matrix4 class has some methods
to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
\param projection: The new projection matrix of the camera. */
void CCameraSceneNode::setProjectionMatrix(const core::matrix4& projection, bool isOrthogonal, bool isDynamicTarget)
{
	IsOrthogonal = isOrthogonal;
	IsDynamicTarget = isDynamicTarget;
	ViewArea.getTransform(video::ETS_PROJECTION) = projection;
}

void CCameraSceneNode::setViewMatrix(const core::matrix4& view)
{
	ViewArea.getTransform(video::ETS_VIEW) = view;
}

//! Gets the current projection matrix of the camera
//! \return Returns the current projection matrix of the camera.
const core::matrix4& CCameraSceneNode::getProjectionMatrix() const
{
	return ViewArea.getTransform ( video::ETS_PROJECTION );
}


//! Gets the current view matrix of the camera
//! \return Returns the current view matrix of the camera.
const core::matrix4& CCameraSceneNode::getViewMatrix() const
{
	return ViewArea.getTransform ( video::ETS_VIEW );
}


//! Sets a custom view matrix affector. The matrix passed here, will be
//! multiplied with the view matrix when it gets updated.
//! This allows for custom camera setups like, for example, a reflection camera.
/** \param affector: The affector matrix. */
void CCameraSceneNode::setViewMatrixAffector(const core::matrix4& affector)
{
	Affector = affector;
}


//! Gets the custom view matrix affector.
const core::matrix4& CCameraSceneNode::getViewMatrixAffector() const
{
	return Affector;
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CCameraSceneNode::OnEvent(const SEvent& event)
{
	if (!InputReceiverEnabled)
		return false;

	// send events to event receiving animators

	ISceneNodeAnimatorList::Iterator ait = Animators.begin();

	for (; ait != Animators.end(); ++ait)
		if ((*ait)->isEventReceiverEnabled() && (*ait)->OnEvent(event))
			return true;

	// if nobody processed the event, return false
	return false;
}


//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CCameraSceneNode::setTarget(const core::vector3df& pos)
{
	Target = pos;
	ShakeTarget = pos;

	if(TargetAndRotationAreBound)
	{
		const core::vector3df toTarget = Target - getAbsolutePosition();
		ISceneNode::setRotation(toTarget.getHorizontalAngle());
	}
}


//! Sets the rotation of the node.
/** This only modifies the relative rotation of the node.
If the camera's target and rotation are bound ( @see bindTargetAndRotation() )
then calling this will also change the camera's target to match the rotation.
\param rotation New rotation of the node in degrees. */
void CCameraSceneNode::setRotation(const core::vector3df& rotation)
{
	if(TargetAndRotationAreBound)
		Target = getAbsolutePosition() + rotation.rotationToDirection();

	ISceneNode::setRotation(rotation);
}


//! Gets the current look at target of the camera
//! \return Returns the current look at target of the camera
const core::vector3df& CCameraSceneNode::getTarget() const
{
	return Target;
}

const core::vector3df& CCameraSceneNode::getShakeTarget() const
{
	return ShakeTarget;
}

//! sets the up vector of the camera
//! \param pos: New upvector of the camera.
void CCameraSceneNode::setUpVector(const core::vector3df& pos)
{
	UpVector = pos;
}


//! Gets the up vector of the camera.
//! \return Returns the up vector of the camera.
const core::vector3df& CCameraSceneNode::getUpVector() const
{
	return UpVector;
}


f32 CCameraSceneNode::getNearValue() const
{
	return ZNear;
}


f32 CCameraSceneNode::getFarValue() const
{
	return ZFar;
}


f32 CCameraSceneNode::getAspectRatio() const
{
	return Aspect;
}


f32 CCameraSceneNode::getFOV() const
{
	return Fovy;
}


void CCameraSceneNode::setNearValue(f32 f)
{
	ZNear = f;
	recalculateProjectionMatrix();
	ViewArea.setFarNearDistance(ZFar - ZNear);
}


void CCameraSceneNode::setFarValue(f32 f)
{
	ZFar = f;
	recalculateProjectionMatrix();
	ViewArea.setFarNearDistance(ZFar - ZNear);
}


void CCameraSceneNode::setAspectRatio(f32 f)
{
	Aspect = f;
	recalculateProjectionMatrix();
}


void CCameraSceneNode::setFOV(f32 f)
{
	Fovy = f;
	recalculateProjectionMatrix();
}


void CCameraSceneNode::recalculateProjectionMatrix()
{
	ViewArea.getTransform ( video::ETS_PROJECTION ).buildProjectionMatrixPerspectiveFovLH(Fovy, Aspect, ZNear, ZFar);
}


//! prerender
void CCameraSceneNode::OnRegisterSceneNode()
{
	if ( SceneManager->getActiveCamera () == this )
		SceneManager->registerNodeForRendering(this, ESNRP_CAMERA);

	ISceneNode::OnRegisterSceneNode();
}


//! render
void CCameraSceneNode::render()
{	
	if (IsOrthogonal == false || IsDynamicTarget == true)
		updateMatrices();

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if ( driver)
	{
		driver->setTransform(video::ETS_PROJECTION, ViewArea.getTransform ( video::ETS_PROJECTION) );
		driver->setTransform(video::ETS_VIEW, ViewArea.getTransform ( video::ETS_VIEW) );
	}
}

//! update
void CCameraSceneNode::updateMatrices()
{
	core::vector3df pos = getAbsolutePosition();
	core::vector3df tgtv = Target - pos;
	tgtv.normalize();

	// if upvector and vector to the target are the same, we have a
	// problem. so solve this problem:
	core::vector3df up = UpVector;
	up.normalize();

	f32 dp = tgtv.dotProduct(up);

	if ( core::equals(core::abs_<f32>(dp), 1.f) )
	{
		up.X += 0.5f;
	}

	// pham hong duc modify
    // rotate target by setRotate
    // apply when shake camera
    core::vector3df t = Target - pos;
    float length = t.getLength();
    core::quaternion q(getRotation());
    t = q * t;
	t.normalize();

	// make camera shake
	core::vector3df right = up.crossProduct(t);

	float time = (float)os::Timer::getTime();
	LastTime = time;
	

	// todo: rotate random target (left - right)
	if (ShakeRandom == true)	
		ShakeX = os::Randomizer::frand();		 
	
	float sx = -Shake.X + ShakeX*(Shake.X*2.0);
	q.fromAngleAxis(sx*core::DEGTORAD, core::vector3df(up));	
	t = q * t;
	t.normalize();

	// todo: rotate random target (up - down)
	if (ShakeRandom == true)	
		ShakeZ = os::Randomizer::frand();		
	
	float sz = -Shake.Z + ShakeZ*(Shake.Z*2.0);
	q.fromAngleAxis(sz*core::DEGTORAD, core::vector3df(right));
	t = q * t;
	t.normalize();

	core::vector3df realTarget = pos + t*length;
	
	// todo: rotate random up vector
	if (ShakeRandom == true)	
		ShakeY = os::Randomizer::frand();
	
	float sy = -Shake.Y + ShakeY*(Shake.Y*2.0);	
	core::vector3df front = realTarget - pos;
	front.normalize();

	// todo: shake near distance
	if (ShakeRandom == true)
		ShakeD = 0.5f + 0.5f*os::Randomizer::frand();

	pos = pos + ShakeD*ShakeDis*front;

	q.fromAngleAxis(sy*core::DEGTORAD, core::vector3df(front));
	up = q * up;
	up.normalize();	

	// todo shake target
	ShakeTarget = realTarget;

	ViewArea.getTransform(video::ETS_VIEW).buildCameraLookAtMatrixLH(pos, Target, up);
	ViewArea.getTransform(video::ETS_VIEW) *= Affector;
	recalculateViewArea();
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCameraSceneNode::getBoundingBox() const
{
	return ViewArea.getBoundingBox();
}


//! returns the view frustum. needed sometimes by bsp or lod render nodes.
const SViewFrustum* CCameraSceneNode::getViewFrustum() const
{
	return &ViewArea;
}


void CCameraSceneNode::recalculateViewArea()
{
	ViewArea.cameraPosition = getAbsolutePosition();

	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
	m.setbyproduct_nocheck(ViewArea.getTransform(video::ETS_PROJECTION),
						ViewArea.getTransform(video::ETS_VIEW));
	ViewArea.setFrom(m);
}


//! Writes attributes of the scene node.
void CCameraSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	ICameraSceneNode::serializeAttributes(out, options);

	out->addVector3d("Target", Target);
	out->addVector3d("UpVector", UpVector);
	out->addFloat("Fovy", Fovy);
	out->addFloat("Aspect", Aspect);
	out->addFloat("ZNear", ZNear);
	out->addFloat("ZFar", ZFar);
	out->addBool("Binding", TargetAndRotationAreBound);
	out->addBool("ReceiveInput", InputReceiverEnabled);
}

//! Reads attributes of the scene node.
void CCameraSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	ICameraSceneNode::deserializeAttributes(in, options);

	Target = in->getAttributeAsVector3d("Target");
	UpVector = in->getAttributeAsVector3d("UpVector");
	Fovy = in->getAttributeAsFloat("Fovy");
	Aspect = in->getAttributeAsFloat("Aspect");
	ZNear = in->getAttributeAsFloat("ZNear");
	ZFar = in->getAttributeAsFloat("ZFar");
	TargetAndRotationAreBound = in->getAttributeAsBool("Binding");
	if ( in->findAttribute("ReceiveInput") )
		InputReceiverEnabled = in->getAttributeAsBool("InputReceiverEnabled");

	recalculateProjectionMatrix();
	recalculateViewArea();
}


//! Set the binding between the camera's rotation adn target.
void CCameraSceneNode::bindTargetAndRotation(bool bound)
{
	TargetAndRotationAreBound = bound;
}


//! Gets the binding between the camera's rotation and target.
bool CCameraSceneNode::getTargetAndRotationBinding(void) const
{
	return TargetAndRotationAreBound;
}


//! Creates a clone of this scene node and its children.
ISceneNode* CCameraSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	ICameraSceneNode::clone(newParent, newManager);

	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CCameraSceneNode* nb = new CCameraSceneNode(newParent,
		newManager, ID, RelativeTranslation, Target);

	nb->ISceneNode::cloneMembers(this, newManager);
	nb->ICameraSceneNode::cloneMembers(this);

	nb->Target = Target;
	nb->UpVector = UpVector;
	nb->Fovy = Fovy;
	nb->Aspect = Aspect;
	nb->ZNear = ZNear;
	nb->ZFar = ZFar;
	nb->ViewArea = ViewArea;
	nb->Affector = Affector;
	nb->InputReceiverEnabled = InputReceiverEnabled;
	nb->TargetAndRotationAreBound = TargetAndRotationAreBound;

	if ( newParent )
		nb->drop();
	return nb;
}


} // end namespace
} // end namespace

