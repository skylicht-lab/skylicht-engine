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

#include "pch.h"
#include "Editor/CEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "GUI/GUI.h"
#include "CAudioPlayerEditor.h"
#include "Activator/CEditorActivator.h"

#ifdef BUILD_SKYLICHT_AUDIO

namespace Skylicht
{
	namespace Editor
	{
		CAudioPlayerEditor::CAudioPlayerEditor() :
			m_emitter(NULL)
		{

		}

		CAudioPlayerEditor::~CAudioPlayerEditor()
		{
			closeGUI();
		}

		void CAudioPlayerEditor::closeGUI()
		{
			CAssetEditor::closeGUI();

			if (m_emitter)
			{
				m_emitter->stop();

				Audio::CAudioEngine* audioEngine = Audio::CAudioEngine::getSoundEngine();
				audioEngine->destroyEmitter(m_emitter);
				m_emitter = NULL;
			}
		}

		void CAudioPlayerEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			CAssetEditor::initGUI(path, ui);

			GUI::CCollapsibleGroup* group = ui->addGroup("Audio info", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);

			Audio::CAudioEngine* audioEngine = Audio::CAudioEngine::getSoundEngine();
			m_emitter = audioEngine->createAudioEmitter(path, false);
			m_emitter->update();

			if (m_emitter && m_emitter->getDecoder())
			{
				GUI::CLabel* label = ui->addValueLabel(layout, L"Name:");
				label->setString(m_emitter->getName());

				Audio::STrackParams track;
				m_emitter->getDecoder()->getTrackParam(&track);

				std::string text;

				label = ui->addValueLabel(layout, L"Decoder:");
				label->setString(m_emitter->getDecoder()->getName());

				label = ui->addValueLabel(layout, L"BitsPerSample:");
				label->setString(std::to_string(track.BitsPerSample));

				label = ui->addValueLabel(layout, L"Channels:");
				label->setString(std::to_string(track.NumChannels));

				label = ui->addValueLabel(layout, L"Sampling Rate:");
				label->setString(std::to_string(track.SamplingRate));
			}
			else
			{
				GUI::CLabel* label = ui->addValueLabel(layout, L"Audio:");
				label->setString("Unknown format!");
			}

			group->setExpand(true);

			group = ui->addGroup("Audio Player", this);
			layout = ui->createBoxLayout(group);
			layout->addSpace(5.0f);

			ui->addButton(layout, L"Play")->OnPress = [&](GUI::CBase* button)
				{
					if (m_emitter)
						m_emitter->play();
				};

			layout->addSpace(5.0f);

			ui->addButton(layout, L"Stop")->OnPress = [&](GUI::CBase* button)
				{
					if (m_emitter)
						m_emitter->stop();
				};

			group->setExpand(true);
		}

		ASSET_EDITOR_REGISTER(CAudioPlayerEditor, wav);
		ASSET_EDITOR_REGISTER(CAudioPlayerEditor, mp3);
	}
}

#endif