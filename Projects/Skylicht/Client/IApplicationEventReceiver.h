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

#pragma once

namespace Skylicht
{

	class IApplicationEventReceiver
	{
	public:
		IApplicationEventReceiver()
		{

		}

		virtual ~IApplicationEventReceiver()
		{

		}

		virtual void onUpdate() = 0;

		virtual void onRender() = 0;

		virtual void onPostRender() = 0;

		virtual void onResume() = 0;

		virtual void onPause() = 0;

		virtual void onInitApp() = 0;

		virtual void onQuitApp() = 0;

		virtual void onResize(int w, int h) = 0;

		// Android
		// - Return false: your app will cancel OS Back Key
		// - Return true: your app will process default OS Back Key
		virtual bool onBack() = 0;

		// Window Editor
		// - Return true: your app will close
		// - Return false: your app will skip os close function
		virtual bool onClose() { return true; }
	};

}