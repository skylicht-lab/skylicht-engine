/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "pch.h"
#include "CListBox.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CListBox::CListBox(CBase* parent) :
				CScrollControl(parent),
				m_multiSelected(false)
			{
				setKeyboardInputEnabled(true);
			}

			CListBox::~CListBox()
			{

			}

			void CListBox::postLayout()
			{
				CBase::postLayout();

				float w = getInnerWidth();
				for (CBase* child : m_innerPanel->Children)
				{
					if (child->width() < w)
						child->setWidth(w);
				}
			}

			CListRowItem* CListBox::addItem(const std::wstring& label, ESystemIcon icon)
			{
				CListRowItem* item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(icon);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}

			CListRowItem* CListBox::addItem(const std::wstring& label)
			{
				CListRowItem* item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(ESystemIcon::None);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}

			void CListBox::removeAllItem()
			{
				m_innerPanel->removeAllChildren();
			}

			CListRowItem* CListBox::getItemByLabel(const std::wstring& label)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item && item->getLabel() == label)
					{
						return item;
					}
				}
				return NULL;
			}

			std::list<CListRowItem*> CListBox::getAllItems()
			{
				std::list<CListRowItem*> result;
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item)
					{
						result.push_back(item);
					}
				}
				return result;
			}

			CListRowItem* CListBox::getItemByTagValue(int value)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item && item->getTagInt() == value)
						return item;
				}
				return NULL;
			}

			void CListBox::onItemDown(CBase* base)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item != NULL)
					{
						if (item == base)
						{
							if (item->getToggle() == false)
							{
								if (OnSelected != nullptr)
									OnSelected(item);

								if (OnSelectChange != nullptr)
									OnSelectChange(item);
							}

							item->setToggle(true);
						}
						else
						{
							if (m_multiSelected)
							{
								if (!CInput::getInput()->isKeyDown(GUI::EKey::KEY_CONTROL))
								{
									// deselected
									if (item->getToggle() == true && OnUnselected != nullptr)
										OnUnselected(item);

									item->setToggle(false);
								}
							}
							else
							{
								// deselected
								if (item->getToggle() == true && OnUnselected != nullptr)
									OnUnselected(item);

								item->setToggle(false);
							}
						}
					}
				}
			}

			CListRowItem* CListBox::getSelected()
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item != NULL && item->getToggle())
					{
						return item;
					}
				}

				return NULL;
			}

			void CListBox::unSelectAll()
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CListRowItem* item = dynamic_cast<CListRowItem*>(child);
					if (item != NULL)
					{
						item->setToggle(false);
					}
				}
			}

			bool CListBox::onKeyUp(bool down)
			{
				if (down)
				{
					CListRowItem* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CListRowItem* item = dynamic_cast<CListRowItem*>(child);
						if (item != NULL)
						{
							if (item->getToggle() == true)
							{
								if (lastItem != NULL && !lastItem->isDisabled())
								{
									if (OnSelected != nullptr)
										OnSelected(lastItem);

									if (OnSelectChange != nullptr)
										OnSelectChange(lastItem);

									lastItem->setToggle(true);
									item->setToggle(false);

									scrollToItem(lastItem);
									return true;
								}
								else
								{
									return true;
								}
							}
						}
						lastItem = item;
					}
				}

				return true;
			}

			bool CListBox::onKeyDown(bool down)
			{
				if (down)
				{
					CListRowItem* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CListRowItem* item = dynamic_cast<CListRowItem*>(child);
						if (item != NULL)
						{
							if (lastItem != NULL && !item->isDisabled())
							{
								if (OnSelected != nullptr)
									OnSelected(item);

								if (OnSelectChange != nullptr)
									OnSelectChange(item);

								item->setToggle(true);
								lastItem->setToggle(false);

								scrollToItem(item);
								return true;
							}
							else if (item->getToggle() == true)
							{
								lastItem = item;
							}
						}
					}
				}
				return true;
			}

			bool CListBox::onKeyHome(bool down)
			{
				if (down)
				{
					CListRowItem* firstItem = NULL;
					CListRowItem* currentSelectItem = NULL;

					for (CBase* child : m_innerPanel->Children)
					{
						CListRowItem* item = dynamic_cast<CListRowItem*>(child);
						if (item != NULL)
						{
							if (firstItem == NULL && !item->isDisabled())
								firstItem = item;

							if (item->getToggle() == true)
							{
								currentSelectItem = item;
								break;
							}
						}
					}

					if (currentSelectItem != NULL &&
						firstItem != NULL &&
						currentSelectItem != firstItem)
					{
						if (OnSelected != nullptr)
							OnSelected(firstItem);

						if (OnSelectChange != nullptr)
							OnSelectChange(firstItem);

						firstItem->setToggle(true);
						currentSelectItem->setToggle(false);

						scrollToItem(firstItem);
					}
				}
				return true;
			}

			bool CListBox::onKeyEnd(bool down)
			{
				if (down)
				{
					CListRowItem* lastItem = NULL;
					CListRowItem* currentSelectItem = NULL;

					for (CBase* child : m_innerPanel->Children)
					{
						CListRowItem* item = dynamic_cast<CListRowItem*>(child);
						if (item != NULL)
						{
							if (item->getToggle() == true)
							{
								currentSelectItem = item;
								break;
							}
						}
					}

					List::reverse_iterator i = m_innerPanel->Children.rbegin(), end = m_innerPanel->Children.rend();
					while (i != end)
					{
						CListRowItem* item = dynamic_cast<CListRowItem*>(*i);
						if (item != NULL && !item->isDisabled())
						{
							lastItem = item;
							break;
						}
						++i;
					}

					if (currentSelectItem != NULL &&
						lastItem != NULL &&
						currentSelectItem != lastItem)
					{
						if (OnSelected != nullptr)
							OnSelected(lastItem);

						if (OnSelectChange != nullptr)
							OnSelectChange(lastItem);

						lastItem->setToggle(true);
						currentSelectItem->setToggle(false);

						scrollToItem(lastItem);
					}
				}
				return true;
			}
		}
	}
}