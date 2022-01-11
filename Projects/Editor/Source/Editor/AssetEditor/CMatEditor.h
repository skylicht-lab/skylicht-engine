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

#include "CAssetEditor.h"
#include "Material/CMaterialManager.h"

namespace Skylicht
{
	namespace Editor
	{
		class CMatEditor : public CAssetEditor
		{
		protected:
			std::vector<ISubject*> m_subjects;

			std::map<CMaterial*, GUI::CBoxLayout*> m_materialUI;

			ArrayMaterial m_materials;
			std::string m_path;

			static GUI::CMenu* s_pickTextureMenu;

		public:
			CMatEditor();

			virtual ~CMatEditor();

			virtual void closeGUI();

			virtual void initGUI(const char* path, CSpaceProperty* ui);

			virtual void onUpdateValue(CObjectSerializable* object);

			void showMaterialGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material);

			static void showShaderGUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, std::vector<ISubject*> subjects, std::function<void()> onChange);

			static void addUniformUI(CSpaceProperty* ui, GUI::CBoxLayout* layout, CMaterial* material, CShader* shader, CShader::SUniformUI* uniformUI, int tab, std::vector<ISubject*> subjects, std::function<void()> onChange);

			DECLARE_GETTYPENAME(CMatEditor);
		};
	}
}