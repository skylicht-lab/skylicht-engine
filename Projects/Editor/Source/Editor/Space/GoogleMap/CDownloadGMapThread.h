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

#include "Thread/IThread.h"
#include "Thread/IMutex.h"
using namespace Skylicht::System;

#include "DownloadMap.h"

#ifdef BUILD_SKYLICHT_NETWORK
#include "HttpRequest/CHttpRequest.h"
using namespace Skylicht::Network;
#endif

#define NUM_HTTPREQUEST	8

namespace Skylicht
{
	namespace Editor
	{
		class CDownloadGMapThread : public IThreadCallback
		{
		protected:
			IThread* m_downloadMapThread;
			IMutex* m_lock;
			IMutex* m_lockFile;

			std::list<SImageDownload> m_queueDownload;
			std::list<SImageDownload> m_downloading;
			std::list<SImageDownload> m_notfound;

			SImageDownload* m_imgDownloading[NUM_HTTPREQUEST];

#ifdef BUILD_SKYLICHT_NETWORK
			CHttpRequest* m_httpRequest[NUM_HTTPREQUEST];
			CHttpStream* m_httpStream[NUM_HTTPREQUEST];
#endif

		public:
			CDownloadGMapThread();

			virtual ~CDownloadGMapThread();

			virtual void updateThread();

			inline void lockReadFile()
			{
				m_lockFile->lock();
			}

			inline void unlockReadFile()
			{
				m_lockFile->unlock();
			}

			void requestDownloadMap(EImageMapType type, long x, long y, int z);

			bool isDownloading(EImageMapType type, long x, long y, int z);

			bool isQueue(EImageMapType type, long x, long y, int z);

			bool isNotFound(EImageMapType type, long x, long y, int z);

			void cancelDownload();
		};
	}
}
