/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CListBase.h"
#include "GUI/Theme/ThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CListBase::CListBase(CBase* parent) :
				CScrollControl(parent),
				m_multiSelected(false)
			{
				setKeyboardInputEnabled(true);
			}

			CListBase::~CListBase()
			{

			}

			void CListBase::removeAllItem()
			{
				m_innerPanel->releaseChildren();
			}

			CButton* CListBase::getItemByLabel(const std::wstring& label)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
					if (item && item->getLabel() == label)
					{
						return item;
					}
				}
				return NULL;
			}

			std::list<CButton*> CListBase::getAllItems()
			{
				std::list<CButton*> result;
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
					if (item)
					{
						result.push_back(item);
					}
				}
				return result;
			}

			CButton* CListBase::getItemByTagValue(int value)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
					if (item && item->getTagInt() == value)
						return item;
				}
				return NULL;
			}

			CButton* CListBase::getSelected()
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
					if (item != NULL && item->getToggle())
					{
						return item;
					}
				}

				return NULL;
			}

			void CListBase::unSelectAll()
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
					if (item != NULL)
					{
						item->setToggle(false);
					}
				}
			}

			void CListBase::onItemDown(CBase* base)
			{
				for (CBase* child : m_innerPanel->Children)
				{
					CButton* item = dynamic_cast<CButton*>(child);
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

			bool CListBase::onKeyUp(bool down)
			{
				if (down)
				{
					CButton* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
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

			bool CListBase::onKeyDown(bool down)
			{
				if (down)
				{
					CButton* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
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

			bool CListBase::onKeyHome(bool down)
			{
				if (down)
				{
					CButton* firstItem = NULL;
					CButton* currentSelectItem = NULL;

					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
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

			bool CListBase::onKeyEnd(bool down)
			{
				if (down)
				{
					CButton* lastItem = NULL;
					CButton* currentSelectItem = NULL;

					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
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
						CButton* item = dynamic_cast<CButton*>(*i);
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