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

#include "CAssetManager.h"
#include "FileWatcher/FileWatcher.h"

namespace Skylicht
{
	namespace Editor
	{
		class CAssetWatcher : public FW::FileWatchListener
		{
		protected:
			FW::FileWatcher* m_fileWatcher;
			FW::WatchID m_watchID;

			CAssetManager* m_assetManager;

			std::list<std::string> m_add;
			std::list<std::string> m_delete;
			std::list<std::string> m_modify;

			std::list<SFileNode*> m_files;

		public:
			CAssetWatcher();

			virtual ~CAssetWatcher();

			void update();

			void beginWatch();

			void endWatch();

			bool needReImport()
			{
				return m_add.size() > 0 || m_files.size() > 0 || m_delete.size() > 0;
			}

			void lock();

			void unlock();

			std::list<SFileNode*>& getFiles()
			{
				return m_files;
			}

			std::list<std::string>& getDeletedFiles()
			{
				return m_delete;
			}

			virtual void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action);
		};
	}
}