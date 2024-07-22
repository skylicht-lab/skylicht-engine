#include "pch.h"
#include "CViewHeader.h"
#include "Context/CContext.h"

#include "Graphics2D/CGUIImporter.h"

CViewHeader::CViewHeader()
{

}

CViewHeader::~CViewHeader()
{

}

void CViewHeader::onInit()
{
	CScene *scene = CContext::getInstance()->getScene();
	CZone *zone = scene->getZone(0);
	
	CGameObject *header = zone->createEmptyObject();
	CCanvas *canvas = header->addComponent<CCanvas>();
	
	CGUIImporter::loadGUI("SampleGUI/Header.gui", canvas);
}

void CViewHeader::onDestroy()
{

}

void CViewHeader::onUpdate()
{
	
}

void CViewHeader::onRender()
{
	
}

void CViewHeader::onGUI()
{
	
}

void CViewHeader::onPostRender()
{
	
}
