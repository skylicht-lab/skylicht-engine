/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CBase.h"
#include "CDockPanel.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CDialogWindow;

			class CCanvas : public CBase
			{
			protected:
				CBase::List m_deleteList;
				std::set<CBase*> m_deleteSet;

				bool m_needSaveDockLayout;

				CDialogWindow* m_dialog;

			public:
				CCanvas(float width, float height);

				virtual ~CCanvas();

				virtual void initialize();

				virtual void doRender();

				virtual void render();

				virtual void update();

				virtual void closeMenu();

				virtual CCanvas* getCanvas();

				virtual void addDelayedDelete(CBase* control);

				virtual void processDelayedDeletes();

				virtual void removeDelayDelete(CBase* control);

				inline void setTopmostDialog(CDialogWindow* dialog)
				{
					m_dialog = dialog;
				}

				inline CDialogWindow* getTopmostDialog()
				{
					return m_dialog;
				}

			public:
				std::function<void(const std::string&)> OnSaveDockLayout;

				inline void notifySaveDockLayout()
				{
					m_needSaveDockLayout = true;
				}

			private:

				void saveDockLayoutToFile();

				void saveDockState(std::string& data, CBase* base, int tab);

				std::string getUTF8String(const std::wstring& s);

				std::string generateTabSpace(int n);

				std::string getRectString(const SRect& r);

			public:

				CBase* FirstTab;
				CBase* NextTab;

			};
		}
	}
}