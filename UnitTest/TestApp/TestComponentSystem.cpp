#include "TestComponentSystem.h"

bool g_initComponentPass = false;
bool g_updateComponentPass = false;
bool g_postUpdateComponentPass = false;
bool g_endUpdateComponentPass = false;
bool g_releaseComponentPass = false;

TestComponent::TestComponent()
{

}

TestComponent::~TestComponent()
{
	g_releaseComponentPass = true;
}

void TestComponent::initComponent()
{
	g_initComponentPass = true;
}

void TestComponent::updateComponent()
{
	g_updateComponentPass = true;
}

void TestComponent::postUpdateComponent()
{
	g_postUpdateComponentPass = true;
}

void TestComponent::endUpdate()
{
	g_endUpdateComponentPass = true;
}

bool isTestComponentPass()
{
	return g_initComponentPass &&
		g_releaseComponentPass &&
		g_updateComponentPass &&
		g_postUpdateComponentPass &&
		g_endUpdateComponentPass;
}