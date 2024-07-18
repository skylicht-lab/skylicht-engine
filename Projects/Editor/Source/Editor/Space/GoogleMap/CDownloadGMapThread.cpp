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
#include "CDownloadGMapThread.h"

using namespace Skylicht::Network;

namespace Skylicht
{
	namespace Editor
	{
		CDownloadGMapThread::CDownloadGMapThread()
		{
			for (int i = 0; i < NUM_HTTPREQUEST; i++)
			{
				m_imgDownloading[i] = NULL;

#ifdef BUILD_SKYLICHT_NETWORK
				m_httpStream[i] = new CHttpStream();
				m_httpRequest[i] = new CHttpRequest(m_httpStream[i]);
#endif
			}

			m_lock = IMutex::createMutex();
			m_lockFile = IMutex::createMutex();
			m_downloadMapThread = IThread::createThread(this);
		}

		CDownloadGMapThread::~CDownloadGMapThread()
		{
			if (m_downloadMapThread != NULL)
				m_downloadMapThread->stop();

			if (m_lock != NULL)
				delete m_lock;

			if (m_lockFile != NULL)
				delete m_lockFile;

			for (int i = 0; i < NUM_HTTPREQUEST; i++)
			{
				m_imgDownloading[i] = NULL;

#ifdef BUILD_SKYLICHT_NETWORK
				m_httpStream[i] = NULL;
				delete m_httpRequest[i];
				m_httpRequest[i] = NULL;
#endif
			}
		}

		void CDownloadGMapThread::updateThread()
		{
			m_lock->lock();

			for (int r = 0; r < NUM_HTTPREQUEST; r++)
			{
				// need create new request
				if (m_queueDownload.size() > 0 && m_imgDownloading[r] == NULL)
				{
					std::list<SImageDownload>::iterator i = m_queueDownload.begin();
					SImageDownload& img = (*i);

#ifdef BUILD_SKYLICHT_NETWORK
					m_httpRequest[r]->setURL(img.Url.c_str());
					m_httpRequest[r]->sendRequestByGet();
#endif

					// push queue to downloading
					m_downloading.push_back((*i));
					m_imgDownloading[r] = &(m_downloading.back());

					m_queueDownload.pop_front();
				}
#ifdef BUILD_SKYLICHT_NETWORK
				else
				{
					// update request download
					if (m_imgDownloading[r] && m_httpRequest[r]->updateRequest() == true)
					{
						if (m_httpRequest[r]->getResponseCode() > 0)
						{
							std::string path = getMapTileLocalCache(
								m_imgDownloading[r]->Image.Type,
								m_imgDownloading[r]->Image.X,
								m_imgDownloading[r]->Image.Y,
								m_imgDownloading[r]->Image.Z);

							const unsigned char* data = m_httpStream[r]->getData();

							bool isPNG = false;
							bool isJPEG = false;

							static unsigned char PNGSignature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
							if (memcmp(data, PNGSignature, sizeof(PNGSignature)) == 0)
								isPNG = true;

							static unsigned char JPGSignature[] = { 0xFF, 0xD8, 0xFF };
							if (memcmp(data, JPGSignature, sizeof(JPGSignature)) == 0)
								isJPEG = true;

							if (isPNG || isJPEG)
							{
								m_lockFile->lock();
								io::IWriteFile* file = getIrrlichtDevice()->getFileSystem()->createAndWriteFile(path.c_str());
								if (file != NULL)
								{
									file->write(m_httpStream[r]->getData(), m_httpStream[r]->getDataSize());
									file->drop();

									m_imgDownloading[r]->Image.Status = EImageMapStatus::Downloaded;
									m_imgDownloading[r]->Image.Path = path;
								}
								else
								{
									m_imgDownloading[r]->Image.Status = EImageMapStatus::NotFound;
									m_notfound.push_back(*m_imgDownloading[r]);
								}
								m_lockFile->unlock();
							}
							else
							{
								m_imgDownloading[r]->Image.Status = EImageMapStatus::NotFound;
								m_notfound.push_back(*m_imgDownloading[r]);
							}
						}

						std::list<SImageDownload>::iterator iDown = m_downloading.begin(), endDown = m_downloading.end();
						while (iDown != endDown)
						{
							if (iDown->Image.Type == m_imgDownloading[r]->Image.Type &&
								iDown->Image.X == m_imgDownloading[r]->Image.X &&
								iDown->Image.Y == m_imgDownloading[r]->Image.Y &&
								iDown->Image.Z == m_imgDownloading[r]->Image.Z)
							{
								// remove on downloading list
								m_downloading.erase(iDown);
								break;
							}
							++iDown;
						}

						m_imgDownloading[r] = NULL;
						m_httpStream[r] = NULL;

						delete m_httpRequest[r];

						m_httpStream[r] = new CHttpStream();
						m_httpRequest[r] = new CHttpRequest(m_httpStream[r]);
					}
				}
#endif
			}
			m_lock->unlock();

			if (m_queueDownload.size() == 0 && m_downloading.size() == 0)
				IThread::sleep(200);
			else
				IThread::sleep(1);
		}

		void CDownloadGMapThread::requestDownloadMap(EImageMapType type, long x, long y, int z)
		{
			m_lock->lock();
			SImageMapElement img;
			img.Type = type;
			img.X = x;
			img.Y = y;
			img.Z = z;

			SImageDownload download;
			download.Image.Type = type;
			download.Image.X = img.X;
			download.Image.Y = img.Y;
			download.Image.Z = img.Z;
			download.Url = getMapTileURL(type, img.X, img.Y, img.Z);

			m_queueDownload.push_back(download);
			m_lock->unlock();
		}

		bool CDownloadGMapThread::isDownloading(EImageMapType type, long x, long y, int z)
		{
			bool ret = false;
			m_lock->lock();
			std::list<SImageDownload>::iterator i = m_downloading.begin(), end = m_downloading.end();
			while (i != end)
			{
				if (i->Image.Type == type &&
					i->Image.X == x &&
					i->Image.Y == y &&
					i->Image.Z == z)
				{
					ret = true;
					break;
				}
				++i;
			}
			m_lock->unlock();
			return ret;
		}

		bool CDownloadGMapThread::isQueue(EImageMapType type, long x, long y, int z)
		{
			bool ret = false;
			m_lock->lock();
			std::list<SImageDownload>::iterator i = m_queueDownload.begin(), end = m_queueDownload.end();
			while (i != end)
			{
				if (i->Image.Type == type &&
					i->Image.X == x &&
					i->Image.Y == y &&
					i->Image.Z == z)
				{
					ret = true;
					break;
				}
				++i;
			}
			m_lock->unlock();
			return ret;
		}

		bool CDownloadGMapThread::isNotFound(EImageMapType type, long x, long y, int z)
		{
			bool ret = false;
			m_lock->lock();
			std::list<SImageDownload>::iterator i = m_notfound.begin(), end = m_notfound.end();
			while (i != end)
			{
				if (i->Image.Type == type &&
					i->Image.X == x &&
					i->Image.Y == y &&
					i->Image.Z == z)
				{
					ret = true;
					break;
				}
				++i;
			}
			m_lock->unlock();
			return ret;
		}

		void CDownloadGMapThread::cancelDownload()
		{
			m_lock->lock();
			m_queueDownload.clear();
			m_notfound.clear();

#ifdef BUILD_SKYLICHT_NETWORK
			for (int i = 0; i < NUM_HTTPREQUEST; i++)
			{
				if (m_httpRequest[i]->isSendRequest())
				{
					m_httpRequest[i]->cancel();
					delete m_httpRequest[i];

					m_httpStream[i] = new CHttpStream();
					m_httpRequest[i] = new CHttpRequest(m_httpStream[i]);
				}
			}
#endif
			m_lock->unlock();
		}
	}
}
