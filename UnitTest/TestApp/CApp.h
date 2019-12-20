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

#ifndef _MAIN_TEST_H_
#define _MAIN_TEST_H_

#include "AppInclude.h"
#include "IApplicationEventReceiver.h"

#include "Scene/CScene.h"

class CApp : public Skylicht::IApplicationEventReceiver
{
private:
	int m_frameCount;

	bool m_passInit;
	bool m_passUpdate;
	bool m_passRender;
	bool m_passPostRender;
	bool m_passQuitApp;
public:
	CApp();

	virtual ~CApp();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual void onInitApp();

	virtual void onQuitApp();

public:

	inline bool isPassInit() { return m_passInit; }

	inline bool isPassUpdate() { return m_passUpdate; }

	inline bool isPassRender() { return m_passRender; }

	inline bool isPassPostRender() { return m_passPostRender; }

	inline bool isPassQuitApp() { return m_passQuitApp; }
};

#endif