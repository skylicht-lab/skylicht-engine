/*
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

#pragma once

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"

#include "Thread/IThread.h"
#include "Thread/IMutex.h"
#include "DownloadMap.h"

#ifdef HAVE_SKYLICHT_NETWORK
#include "HttpRequest/CHttpRequest.h"
#endif

#define NUM_HTTPREQUEST	8

namespace Skylicht
{
	namespace Editor
	{
		struct SRenderMap
		{
			core::vector2d<long> From;
			core::vector2d<long> To;
			long CountX;
			long CountY;

			SRenderMap()
			{
				CountX = 0;
				CountY = 0;
			}
		};

		struct SExportRect
		{
			int MouseHit;
			bool Ready;
			double Lat1;
			double Lng1;
			double Lat2;
			double Lng2;

			SExportRect()
			{
				MouseHit = 0;
				Ready = false;
				Lat1 = 0.0;
				Lng1 = 0.0;
				Lat2 = 0.0;
				Lng2 = 0.0;
			}
		};

		class CSpaceGMap : public CSpace, IThreadCallback
		{
		protected:
			GUI::CBase* m_view;

			int m_zoom;
			long m_viewX;
			long m_viewY;

			SRenderMap m_renderMap;
			int m_gridSize;

			int m_materialID;

			CGlyphFont* m_fontNormal;
			CGlyphFont* m_fontLarge;

			bool m_rightPress;
			bool m_leftPress;

			IThread* m_downloadMapThread;
			IMutex* m_lock;
			IMutex* m_lockFile;

			std::list<SImageDownload> m_queueDownload;
			std::list<SImageDownload> m_downloading;
			std::list<SImageDownload> m_notfound;

			SImageDownload* m_imgDownloading[NUM_HTTPREQUEST];
			CHttpRequest* m_httpRequest[NUM_HTTPREQUEST];
			CHttpStream* m_httpStream[NUM_HTTPREQUEST];

			EImageMapType m_mapBGType;

			std::vector<SImageMapElement> m_mapOverlay;

			GUI::CLabel* m_zoomLabel;
			GUI::CButton* m_btnExportRect;
			GUI::CButton* m_btnRemoveExport;
			GUI::CButton* m_btnExport;

			SExportRect m_exportRect;
			GUI::SPoint m_mouseLocal;

		public:
			CSpaceGMap(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceGMap();

			void clear();

			void onAddExportArea(GUI::CBase* base);

			void onRemoveExport(GUI::CBase* base);

			void onExport(GUI::CBase* base);

			void exportMap(const char* path);

			virtual void updateThread();

			virtual void onResize(float w, float h);

			virtual void onRender(GUI::CBase* base);

			virtual void onMouseMoved(GUI::CBase* base, float x, float y, float deltaX, float deltaY);

			virtual void onLeftMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onRightMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMiddleMouseClick(GUI::CBase* base, float x, float y, bool down);

			virtual void onMouseWheeled(GUI::CBase* base, int wheel);

		protected:

			int getMapState(long x, long y, int z);

			void requestDownloadMap(long x, long y, int z);

			ITexture* searchMapTileset(long x, long y, int z);

			ITexture* searchMapTilesetOnLocal(long x, long y, int z);

			void updateMap();

			void renderMap();

			void renderMapBG();

			void renderExportArea();

			core::rectf getExportRectInVP();

			void renderGrid();

			void renderString();

			void zoomIn(long viewX, long viewY);

			void zoomOut(long viewX, long viewY);

			void cancelDownload();

			void updateZoomString();
		};
	}
}