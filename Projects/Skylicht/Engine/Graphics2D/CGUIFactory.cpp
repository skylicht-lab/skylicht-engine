#include "pch.h"
#include "CGUIFactory.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGUIFactory);

	CGUIFactory::CGUIFactory()
	{

	}

	CGUIFactory::~CGUIFactory()
	{

	}

	void CGUIFactory::addPlugin(const char* name, std::function<CGUIElement* (CGUIElement*)> func)
	{
		m_createGUIFunc[name] = func;
	}

	CGUIElement* CGUIFactory::createGUI(const char* name, CGUIElement* parent)
	{
		if (m_createGUIFunc.find(name) == m_createGUIFunc.end())
			return NULL;

		return m_createGUIFunc[name](parent);
	}
}