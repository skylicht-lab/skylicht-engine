/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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

#pragma once

#include "Serializable/CObjectSerializable.h"
#include "Reactive/IObserver.h"
#include "Reactive/ISubject.h"
#include "Editor/Components/CComponentEditor.h"
#include "Activator/CEditorActivator.h"

namespace Skylicht
{
	namespace Editor
	{
		class CDefaultEditor : public CComponentEditor
		{
		public:

		protected:
			CObjectSerializable* m_data;

			std::string m_name;

			std::vector<ISubject*> m_subjects;

			std::wstring m_tempName;

		public:
			CDefaultEditor();

			virtual ~CDefaultEditor();

			virtual void initGUI(CComponentSystem* target, CSpaceProperty* spaceProperty);

			virtual void update();

			void setName(const char* name)
			{
				m_name = name;
			}

		protected:

			const wchar_t* getPrettyName(const std::string& paramName);

		};
	}
}