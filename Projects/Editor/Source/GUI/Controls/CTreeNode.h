/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "CBase.h"
#include "CIconTextItem.h"
#include "CIconButton.h"
#include "CTreeRowItem.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTreeNode : public CBase
			{
			protected:
				CIconTextItem *m_title;
				CIconButton *m_expandButton;
				CTreeRowItem *m_row;

				CTreeNode *m_root;
				CTreeNode *m_parentNode;

				bool m_expand;
				bool m_selected;

			public:

				Listener OnSelected;
				Listener OnUnselected;
				Listener OnSelectChange;
				Listener OnItemContextMenu;
			public:
				CTreeNode(CBase *parent, CTreeNode *root);

				virtual ~CTreeNode();

				virtual void layout();

				virtual void postLayout();

				inline CIconTextItem *getTextItem()
				{
					return m_title;
				}

				inline CTreeRowItem* getRowItem()
				{
					return m_row;
				}

				CTreeNode* addNode(const std::wstring& text);

				CTreeNode* addNode(const std::wstring& text, ESystemIcon icon);

				void setText(const std::wstring& text);

				void setIcon(ESystemIcon icon);

				inline bool isExpand()
				{
					return m_expand;
				}

				inline void expand()
				{
					m_expand = true;
					invalidate();
				}

				inline void collapse()
				{
					m_expand = false;
					invalidate();
				}

				inline bool haveChild()
				{
					return m_innerPanel->Children.size() > 0;
				}

				inline bool isSelected()
				{
					return m_selected;
				}

				inline CTreeNode* getParentNode()
				{
					return m_parentNode;
				}

				CTreeNode* getChildSelected();

				CTreeNode* selectFirstChild();

				CTreeNode* selectLastChild();

				CTreeNode* selectNextChild();

				CTreeNode* selectPrevChild();

				void setSelected(bool b);

				virtual void deselectAll();

				bool hoverOnChild();

			protected:

				void onExpand(CBase *base);

				void onDoubleClick(CBase *base);

				void onDown(CBase *base);

				virtual void onNodeClick(CBase *base);
			};
		}
	}
}