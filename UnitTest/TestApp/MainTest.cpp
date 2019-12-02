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

#include "MainTest.h"
#include "TestCoreUtils.hpp"
#include "TestSystemThread.hpp"

MainTest::MainTest()
{
	m_frameCount = 0;

	m_passInit = false;
	m_passUpdate = false;
	m_passRender = false;
	m_passPostRender = false;
	m_passQuitApp = false;
}

MainTest::~MainTest()
{

}

void MainTest::onInitApp()
{
	m_passInit = true;

	testCoreUtils();

	testSystemThread();
}

void MainTest::onUpdate()
{
	m_passUpdate = true;

	// try test in 5 frame
	if (++m_frameCount >= 5)
		getIrrlichtDevice()->closeDevice();
}

void MainTest::onRender()
{
	m_passRender = true;
}

void MainTest::onPostRender()
{
	m_passPostRender = true;
}

void MainTest::onResume()
{

}

void MainTest::onPause()
{

}

void MainTest::onQuitApp()
{
	m_passQuitApp = (m_frameCount == 5);
}

bool MainTest::isThreadPass()
{
	g_thread->stop();
	delete g_thread;
	return g_threadPass;
}