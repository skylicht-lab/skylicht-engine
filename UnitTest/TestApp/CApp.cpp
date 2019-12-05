/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "CApp.h"
#include "TestCoreUtils.h"
#include "TestSystemThread.h"
#include "TestScene.h"

#include "CApplication.h"
#include "Material/CShaderManager.h"

#define TEST_UPDATE_LOOP_COUNT	100

bool g_finalPass = false;

void installApplication(const std::vector<std::string>& argv)
{
	CApp *mainTest = new CApp();
	getApplication()->registerAppEvent("CApp", mainTest);
}

CApp::CApp()
{
	m_frameCount = 0;

	m_passInit = false;
	m_passUpdate = false;
	m_passRender = false;
	m_passPostRender = false;
	m_passQuitApp = false;
}

CApp::~CApp()
{
	g_finalPass = true;
}

void CApp::onInitApp()
{
	m_passInit = true;

	// File system
	io::IFileSystem *fileSystem = getApplication()->getFileSystem();

	// Add built in data
	fileSystem->addFileArchive("BuiltIn.zip", false, false);

	// Load basic shader
	CShaderManager::getInstance()->initBasicShader();

	// Run unit test
	testCoreUtils();

	testSystemThread();

	testScene();
}

void CApp::onUpdate()
{
	m_passUpdate = true;

	testSceneUpdate();
	
	if (++m_frameCount >= TEST_UPDATE_LOOP_COUNT)
		getIrrlichtDevice()->closeDevice();
}

void CApp::onRender()
{
	m_passRender = true;
}

void CApp::onPostRender()
{
	m_passPostRender = true;
}

void CApp::onResume()
{

}

void CApp::onPause()
{

}

void CApp::onQuitApp()
{
	m_passQuitApp = (m_frameCount == TEST_UPDATE_LOOP_COUNT);

	TEST_ASSERT_THROW(isSystemThreadPass());
	TEST_ASSERT_THROW(isTestScenePass());

	TEST_CASE("App init");
	TEST_ASSERT_THROW(this->isPassInit());

	TEST_CASE("App update");
	TEST_ASSERT_THROW(this->isPassUpdate());

	TEST_CASE("App render");
	TEST_ASSERT_THROW(this->isPassRender());

	TEST_CASE("App post render");
	TEST_ASSERT_THROW(this->isPassPostRender());

	TEST_CASE("App quit");
	TEST_ASSERT_THROW(this->isPassQuitApp());

	delete this;
}