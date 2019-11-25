// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "CDefaultSceneNodeFactory.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "IParticleSystemSceneNode.h"
#include "ILightSceneNode.h"
#include "IMeshSceneNode.h"

namespace irr
{
namespace scene
{


CDefaultSceneNodeFactory::CDefaultSceneNodeFactory(ISceneManager* mgr)
: Manager(mgr)
{

	#ifdef _DEBUG
	setDebugName("CDefaultSceneNodeFactory");
	#endif

	// don't grab the scene manager here to prevent cyclic references	
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_EMPTY, "empty"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_CAMERA, "camera"));
	
	// SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_MD3_SCENE_NODE, "md3"));

	// legacy, for version <= 1.4.x irr files
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_CAMERA_MAYA, "cameraMaya"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_CAMERA_FPS, "cameraFPS"));
}


//! adds a scene node to the scene graph based on its type id
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent)
{
	switch(type)
	{	
	case ESNT_LIGHT:
		return Manager->addLightSceneNode(parent);
	case ESNT_EMPTY:
		return Manager->addEmptySceneNode(parent);	
	case ESNT_CAMERA:
		return Manager->addCameraSceneNode(parent);
	case ESNT_CAMERA_MAYA:
		return Manager->addCameraSceneNodeMaya(parent);
	case ESNT_CAMERA_FPS:
		return Manager->addCameraSceneNodeFPS(parent);	
	default:
		break;
	}

	return 0;
}


//! adds a scene node to the scene graph based on its type name
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(const c8* typeName, ISceneNode* parent)
{
	return addSceneNode( getTypeFromName(typeName), parent );
}


//! returns amount of scene node types this factory is able to create
u32 CDefaultSceneNodeFactory::getCreatableSceneNodeTypeCount() const
{
	return SupportedSceneNodeTypes.size();
}


//! returns type of a createable scene node type
ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getCreateableSceneNodeType(u32 idx) const
{
	if (idx<SupportedSceneNodeTypes.size())
		return SupportedSceneNodeTypes[idx].Type;
	else
		return ESNT_UNKNOWN;
}


//! returns type name of a createable scene node type
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(u32 idx) const
{
	if (idx<SupportedSceneNodeTypes.size())
		return SupportedSceneNodeTypes[idx].TypeName.c_str();
	else
		return 0;
}


//! returns type name of a createable scene node type
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
{
	for (u32 i=0; i<SupportedSceneNodeTypes.size(); ++i)
		if (SupportedSceneNodeTypes[i].Type == type)
			return SupportedSceneNodeTypes[i].TypeName.c_str();

	return 0;
}


ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getTypeFromName(const c8* name) const
{
	for (u32 i=0; i<SupportedSceneNodeTypes.size(); ++i)
		if (SupportedSceneNodeTypes[i].TypeName == name)
			return SupportedSceneNodeTypes[i].Type;

	return ESNT_UNKNOWN;
}


} // end namespace scene
} // end namespace irr

