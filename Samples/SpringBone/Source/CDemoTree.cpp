#include "pch.h"
#include "CDemoTree.h"

CDemoTree::CDemoTree(Verlet::CVerlet* verlet) :
	CDemo(verlet)
{

}

CDemoTree::~CDemoTree()
{

}

void CDemoTree::init()
{
	m_verlet->clear();
}

void CDemoTree::update()
{

}

void CDemoTree::onGUI()
{

}